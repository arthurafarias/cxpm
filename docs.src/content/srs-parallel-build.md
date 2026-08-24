---
title: "SRS: Parallel Build"
---

# Software Requirements Specification
## Parallelizing the Build Pipeline

**Project:** cxpm
**Status:** **Proposed — not yet implemented.** Unlike its companion SRS pages, this document
specifies work that has not landed. It exists to pin down the design *before* implementation,
against the codebase as it stands as of this writing, so the eventual change can be reviewed
against a stated baseline rather than reverse-engineered from a diff.
**Scope:** `CXPM::Toolchain::build` (both overloads), `Controllers::ProjectManager::build_project`
(`libraries/cxpm-interface/src/CXPM/ProjectManager.hpp`), `Views::ApplicationView`'s `--build` flag
handling, `Threading::ThreadPool` (`Core/Threading/ThreadPool.hpp`)
**Companion documents:** [architecture.md](architecture.md), [SRS-architecture.md](srs-architecture.md)
items A21 (JIT-compilation caching — complementary, not a substitute, for this SRS: caching avoids
recompiling toolchains that haven't changed, parallelism speeds up the compiles that do still have
to happen), A22 (toolchain-plugin naming collision — a hard prerequisite for §3.4), A24
(`BasicCollection` is not thread-safe), A28 (`ShellManager::exec`'s `popen`-based process spawn),
A37 (toolchain-registry population ordering); [SRS-sandbox.md](srs-sandbox.md) (the existing
`ThreadPool`/`posix_spawn` rationale this SRS builds on)

---

## 1. Purpose

Object-file compilation for a *single* target is already parallel: `Toolchain::build(const
TargetDescriptor&)` fires one `object_build_async` task per source file
(`Toolchain.hpp:629-641`) before waiting on any of them. Nothing else in the build pipeline is:

- Every **target** in a project is built strictly sequentially, in two separate places that
  duplicate the same loop shape:
  - `ProjectManager::build_project`'s `for (auto target : project_manifest.targets)`
    (`ProjectManager.hpp:122-150`) — the actual path `cxpm --build` runs.
  - `Toolchain::build(const ProjectDescriptor&)`'s `for (auto package : project.targets)`
    (`Toolchain.hpp:600-610`) — a library-level convenience entry point, exercised directly by
    `ToolchainTest`, not currently called from `build_project`.
- Every **toolchain plugin** (`toolchain.cpp`) discovered by `ToolchainManager::autoscan` is
  JIT-compiled one at a time (`ToolchainManager.hpp:153-198`).
- Object-file compilation itself is unbounded: each source file gets its own raw
  `std::async(std::launch::async, ...)` thread (`Toolchain.hpp:356`), with no cap. A target with
  200 source files spawns 200 OS threads simultaneously, independent of the machine's actual core
  count, and independent of however many *other* targets are also compiling at the same time.

This SRS specifies how to parallelize target builds and bound the total concurrency across the
whole build — object compiles *and* target builds together — through the `ThreadPool` the codebase
already has, rather than continuing to add more unbounded `std::async` call sites.

## 2. Baseline: what parallelizes safely today, and why

**Targets within a project have no dependency graph.** `TargetDescriptor::dependencies`
(`TargetDescriptor.hpp:20`) is a flat list of *external* package names — its only consumer is
`Controllers::PackageConfigManager::find_package`, called once per target to resolve `pkg-config`
flags (`Toolchain.hpp:620-627`). There is no field anywhere in `ProjectDescriptor`/
`TargetDescriptor` that lets one target in a project declare a build-order dependency on another
target in the same project. This is the load-bearing fact behind this SRS: **every target in a
project is, today, an independent build unit**, so parallelizing across targets requires no
topological sort, no dependency resolution, and no new data on `TargetDescriptor` — it is safe
with the schema exactly as it stands. (If a future change adds real target-to-target dependencies —
see [roadmap.md](roadmap.md) item 13 — this SRS's target-level requirements below would need
revisiting; §4 flags this explicitly as the one assumption that could later invalidate them.)

**The primitives this SRS needs already exist and are already safe to share across threads:**

- `Threading::ThreadPool::get_instance()` — a bounded worker pool (default
  `std::thread::hardware_concurrency()` threads), already used by `ShellManager::exec_async`
  (`ShellManager.hpp:72-85`) and chosen over `fork()` in [SRS-sandbox.md §3.2.3](srs-sandbox.md)
  precisely because it doesn't duplicate a multithreaded process's lock state.
- `Core::Logging::LoggerManager` logs through `std::osyncstream` (`LoggerManager.hpp:32`), which
  is safe for concurrent writers out of the box — nothing to change here.
- `Utils::Unix::ShellManager::exec` spawns one `popen`'d child process per call; concurrent calls
  from different threads don't share state (see A28 for `exec`'s own, unrelated, injection-safety
  issues, which this SRS neither introduces nor fixes).

**What is *not* safe to share across threads without care:**

- `Core::Containers::BasicCollection` (A24) — privately inherits a `Lockable` it never actually
  locks. It looks thread-aware; it provides no synchronization. This SRS's requirements are written
  so that no worker thread ever mutates a `BasicCollection` another thread can also see live — see
  §3.3.
- `ToolchainManager::toolchains` — a function-local `static` `BasicCollection<ToolchainDescriptor>`
  mutated in place by `autoscan` and read by `by_name`/`autoselect`/`current`
  (`ToolchainManager.hpp:53-123`). `build_project` already calls `ToolchainManager::current(...)`
  — which runs `autoscan` — exactly once, up front, before any target build starts (this ordering
  is itself the fix for A37, already landed). §3.2 requires that this ordering is preserved: the
  parallel target-build phase begins only *after* the toolchain registry is fully populated, and no
  worker thread calls anything that re-triggers `autoscan` while target builds are in flight.

## 3. Requirements

### 3.1 A shared, bounded job budget

1. `Threading::ThreadPool` SHALL remain the single pool of worker threads used for all build
   concurrency — object compiles, links, and target builds alike. No new call site introduced by
   this SRS SHALL spawn an unbounded `std::async(std::launch::async, ...)` thread the way
   `object_build_async` does today.
2. `Toolchain::object_build_async` (`Toolchain.hpp:352-361`) SHALL be re-implemented on top of
   `Threading::ThreadPool::get_instance()` (matching the pattern `ShellManager::exec_async` already
   uses: submit a lambda, resolve a `std::promise` from inside it, return the shared future) instead
   of a raw `std::async` call, so a target with many source files no longer spawns one OS thread per
   source file.
3. The pool's size SHALL be configurable as a build job budget, honored by the CLI's `--build`
   path:
   - A new `--jobs`/`-j <N>` option on `cxpm`, following the existing long/short pairing convention
     (`--build`/`-b`, `--install`/`-i`, `--generate`/`-g`) in `ApplicationView::run()`.
   - Falling back to a `CXPM_BUILD_JOBS` environment variable when the flag is absent.
   - Defaulting to `std::thread::hardware_concurrency()` (the same default `ThreadPool`'s
     constructor already uses) when neither is set.
   - `N = 1` SHALL produce build behavior indistinguishable in outcome from today's fully
     sequential code path — this is the escape hatch for anyone who hits a concurrency-related
     regression, in the same spirit as `CXPM_SANDBOX_DISABLE=1` in
     [SRS-sandbox.md §3.2.4](srs-sandbox.md).
4. This job budget is **total, not per-level**. Targets building concurrently and the object files
   each of them is concurrently compiling all draw from the same `ThreadPool`, so a project with 4
   targets of 50 sources each does not attempt 200 simultaneous compiler invocations under a
   `--jobs 8` budget — it attempts 8, exactly as `make -j8` would. This falls out for free from
   routing both levels through one pool rather than needing separate accounting.

### 3.2 Target-level parallelism

1. `ProjectManager::build_project`'s per-target loop (`ProjectManager.hpp:122-150`) SHALL submit
   one build task per target to `Threading::ThreadPool` and wait for all of them, instead of
   building targets one after another. Per-target toolchain selection
   (`ToolchainManager::autoselect`/`by_name`) SHALL still happen — per §2, only *after* the
   up-front `ToolchainManager::current(...)` call has finished populating the registry, i.e., the
   existing structural fix for A37 is preserved unchanged; only the loop body's execution model
   changes.
2. `Toolchain::build(const ProjectDescriptor&)` (`Toolchain.hpp:594-613`) SHALL be parallelized the
   same way, for the same reason (§2: no target has a same-project dependency to respect).
3. Each submitted target-build task SHALL own its own, independently-copied `Toolchain` value (the
   type is a small value object — compiler paths, flag strings, no shared mutable state) rather
   than closing over a toolchain variable shared with, or reassigned by, other in-flight tasks.
   This replaces `build_project`'s current pattern of reassigning one loop-scoped `Toolchain
   toolchain;` on every iteration (`ProjectManager.hpp:86,125,128`), which is fine under strict
   sequential execution but would be a data race the moment two iterations run concurrently.

### 3.3 Result aggregation stays single-threaded

1. No worker task SHALL mutate a `BasicCollection` (or any other non-thread-safe shared state)
   directly. Each target-build task SHALL return its own, task-local result (status +
   `BasicCollection<CompileCommandDescriptor>`); the orchestrating thread SHALL merge these into
   the shared `commands`/`result` collection only after joining (`.get()`), one task's result at a
   time — mirroring the pattern `Toolchain::build(const TargetDescriptor&)` already uses for its
   per-source futures (`Toolchain.hpp:629-641`: every future is dispatched first, then drained in a
   second, single-threaded loop).
2. The merge order SHALL be **submission order** (i.e., `project_manifest.targets`' declared
   order), not completion order, so that:
   - `compile_commands.json`'s emitted content (`ProjectManager.hpp:152-163`) stays deterministic
     across runs regardless of which target's compiler happens to finish first — required for
     reproducible builds and for any test that asserts on its exact contents.
   - `BuildProjectOutputResult`'s returned `ToolchainDescriptor` (`ProjectManager.hpp:165`, third
     tuple element) stays exactly what it is today: the toolchain resolved for the *last* target in
     manifest order. Today that's an accident of sequential execution (whichever iteration ran
     last); this SRS makes it an explicit, order-based rule so the return value doesn't become
     nondeterministic once targets no longer complete in declaration order.
3. Failure handling changes from "stop at the first target that fails" to "let every already-
   submitted target finish, then report failure if any did." Neither `ThreadPool` nor `std::async`
   offers a cancellation primitive, so a submitted task cannot be un-submitted once dispatched —
   this SRS does not add one. Concretely:
   - Overall `Status` SHALL be `Failure` if any target's build failed, `Success` otherwise.
   - The aggregated `BasicCollection<CompileCommandDescriptor>` SHALL still include every target's
     commands (successful or not), in submission order per §3.3.2 — not just the first failure's,
     the way `Toolchain::build(const TargetDescriptor&)`'s existing early-`return` on the first
     failing *source* still does today for that inner, per-target loop (which this SRS leaves as
     "return on first failing source within a target" — see §4, that narrower behavior is
     unchanged).
   - `ProjectManager::build_project`'s existing per-target `try`/`catch` around
     `toolchain.build(target)` (`ProjectManager.hpp:123,146-149`, which logs and continues rather
     than aborting the whole build on one target's exception) SHALL be preserved inside each
     parallel task, unchanged.

### 3.4 Toolchain-plugin discovery: explicitly out of scope

`ToolchainManager::autoscan`'s own compile loop (`ToolchainManager.hpp:153-198`) is **not**
parallelized by this SRS. `build_toolchain_plugin` derives each generated artifact's filename from
`source_path.parent_path().filename()` (`ToolchainManager.hpp:213`) — a bug already itemized as A22
— which today produces the *same* filename (`"src"`) for all three in-tree toolchains
(`gcc/src/toolchain.cpp`, `g++/src/toolchain.cpp`, `nvcc/src/toolchain.cpp`). The current code only
gets away with this because each plugin fully compiles, loads, and `dlclose`s before the next one
starts. Parallelizing this specific loop without first landing A22's fix would turn a latent,
already-documented bug into a real, timing-dependent race: two threads writing the same
`toolchain-src.loader.cpp`/`libtoolchain-src.so` paths concurrently. **A22 SHALL land before this
loop is parallelized**; that follow-up is tracked as future work (§4), not delivered here.

## 4. Known limitations / accepted for this iteration

- **No target-to-target dependency support, by design.** §2 establishes that today's schema has
  none, so parallelizing target builds is unconditionally safe. The moment
  [roadmap.md](roadmap.md) item 13 (a real dependency-resolution story) adds one, §3.2's
  "build every target concurrently" requirement stops being universally correct and this SRS
  needs a follow-up (a real scheduler respecting the new graph, not just "submit everything at
  once").
- **`ToolchainManager::autoscan`'s toolchain-plugin compile loop stays sequential** (§3.4),
  blocked on A22. Once A22 lands, extending this SRS's job-budget model to that loop is a natural,
  small follow-up — tracked here rather than bundled in, since it touches different, currently-
  buggy code.
- **Within a single target, a failing source file still short-circuits that target's own link
  step** exactly as it does today (`Toolchain.hpp:635-641`: the per-source result loop returns as
  soon as it walks to a failing entry, in submission order, without waiting on later entries in
  that same target — though, per §3.1.2, those later entries are still in flight against the
  shared pool and will run to completion regardless, since there is no cancellation). This SRS
  changes that loop's *implementation* (raw `std::async` → `ThreadPool`) but not this
  already-existing "first failing source in submission order wins" behavior, since no test or
  documented requirement calls for the more aggressive "cancel every future" semantics.
- **No `--jobs` value validation is specified beyond "falls back to `hardware_concurrency()` when
  unset."** Whether `--jobs 0` or a negative value should error out or silently clamp is left to
  implementation-time judgment; it is not a design question this SRS needs to settle.
- **This SRS does not address A28** (unescaped `popen`+shell-string command construction). Running
  more compiler invocations concurrently does not change that finding's severity or scope; it is
  listed here only because §2 leans on `ShellManager::exec` being safe to call concurrently, which
  is a distinct property from the injection issue A28 describes.

## 5. Testing requirements

| Requirement | How it should be verified |
|---|---|
| §3.1.2/§3.1.3: object-file compilation is bounded by the configured job budget, not one thread per source file | A dry-run (`dry=true`) unit test building a target with more sources than `hardware_concurrency()` returns the same per-source results as today, exercised through `ThreadPoolTest`-style instrumentation (e.g., a counting/blocking stub task) rather than timing, matching this project's existing preference (see [SRS-sandbox.md §4](srs-sandbox.md#4-known-limitations-accepted-for-this-iteration)) for behavioral assertions over wall-clock ones in CI |
| §3.1.3: `--jobs`/`-j` and `CXPM_BUILD_JOBS` are honored, `--jobs 1` matches pre-parallelization sequential output | A `ProgramOptions::Parse`-level unit test for the new flag/short-flag pair (mirroring `ApplicationView`'s existing `--build`/`-b` handling), plus one CLI integration test building a small multi-target fixture with `--jobs 1` and diffing its `compile_commands.json` against the pre-parallelization baseline |
| §3.2.1/§3.2.2: all targets in a project build even though none of them block on another | Replace `ToolchainTest`'s current "build(ProjectDescriptor, dry=true) stops at the first target that fails" case (`ToolchainTest.hpp`) — "first" is not a well-defined concept once targets build concurrently — with an assertion that the aggregated result is `Status::Failure` and still contains every target's command, per §3.3.3 |
| §3.2.3: each task uses its own `Toolchain` copy, not a shared/reassigned one | A unit test building ≥2 targets that resolve to *different* toolchains (e.g., different `target.toolchain` names) concurrently, asserting each target's recorded `CompileCommandDescriptor.command` used the correct compiler executable for *its own* target, not whichever toolchain another in-flight task happened to select |
| §3.3.2: `compile_commands.json` content and the returned `ToolchainDescriptor` are deterministic across runs | Run the same multi-target fixture's `cxpm --build` several times (or dry-run build several times with artificially staggered per-target delays via a test toolchain), asserting byte-identical `compile_commands.json` output and an unchanged third tuple element from `build_project` every time |
| §3.4: the toolchain-discovery loop is untouched by this change | The existing `sandbox_extracts_toolchain_descriptor_as_json` / `cli_build_example_executable*` CTest cases ([SRS-sandbox.md §5](srs-sandbox.md#5-testing)) continue to pass unmodified — no new test needed here, only confirmation nothing in this SRS touches `ToolchainManager::autoscan` |

A CLI-level, multi-target fixture project does not exist under `examples/` or `tests/cli/` today
(every example directory — `archive-object`, `executable`, `executable-json`,
`executable-with-gstreamer-1.0`, `shared-object` — declares exactly one target). Implementing the
integration-level rows above requires adding one (or extending `tests/cli`'s own fixtures, which
`ToolchainTest.hpp`'s unit-level tests already do not need, since they construct `ProjectDescriptor`
values directly in C++ rather than through a real `package.cpp`/`package.json`).

## 6. Acceptance criteria

- `Toolchain::object_build_async` and all target-build submissions route through
  `Threading::ThreadPool::get_instance()`; no new unbounded `std::async(std::launch::async, ...)`
  call site is introduced.
- `cxpm --build` accepts `--jobs`/`-j <N>` and `CXPM_BUILD_JOBS`, defaults to
  `hardware_concurrency()`, and `--jobs 1` reproduces today's sequential build's observable output
  (`compile_commands.json` content, exit status, returned toolchain) exactly.
- Building a multi-target project succeeds with all targets built, `compile_commands.json` content
  deterministic across repeated runs, and overall status correctly reflecting `Failure` if any
  target failed — without stopping early at whichever target happens to fail first under
  concurrent scheduling.
- `ToolchainManager::autoscan`'s toolchain-plugin discovery loop is unmodified by this change (its
  own parallelization is out of scope per §3.4, pending A22).
- Every row in §5 has a corresponding passing test.
