---
layout: default
title: "SRS: Descriptor Sandbox"
---

# Software Requirements Specification
## Sandbox Infrastructure for Building C++ Descriptors

**Project:** cxpm
**Scope:** `applications/cxpm-descriptor-sandbox`, `CXPM::Controllers::DescriptorSandbox`
(`libraries/cxpm-interface/src/CXPM/DescriptorSandbox.hpp`), the two call sites it replaces:
`ProjectManager::load_from_manifest`, `ToolchainManager::load_toolchain_plugin`
**Companion documents:** [architecture.md](architecture.md), [SRS-json-manifests.md](SRS-json-manifests.md) (the codec used as the IPC format), [SRS-architecture.md](SRS-architecture.md) items A28/A21 (the trust-boundary and performance context this responds to)

---

## 1. Purpose

`package.cpp` and `toolchain.cpp` are compiled into a shared object and `dlopen`'d — before this
SRS, directly into the running `cxpm` process. That `.so` was compiled moments earlier from a
`.cpp` file the project (or one of its dependencies) supplied; the instant `dlopen()` returns, its
static initializers have already run with the full privileges of the `cxpm` process — file
access, network, environment, everything. There was no boundary between "code cxpm shipped" and
"code a project/dependency author wrote."

This SRS specifies a sandbox: the two `dlopen()` call sites that load a *build product of an
arbitrary, potentially third-party `.cpp` file* are replaced with a call into a disposable child
process, bounded by resource limits, whose only communication channel back to the parent is a
JSON-encoded descriptor on stdout.

## 2. Scope: what is, and is not, sandboxed

| Loaded shared object | Sandboxed? | Why |
|---|---|---|
| `libproject-manifest.so` (compiled from the project's own `package.cpp`) | **Yes** | Directly authored by the project — the primary case this SRS targets |
| `libtoolchain-<name>.so` (compiled from a discovered `toolchain.cpp`) | **Yes** | May come from a search path outside the project directory (`$HOME/.local/...`, install-prefix-relative paths); same trust concern |
| A `package.json`/`toolchain.json` manifest | **N/A — nothing to sandbox** | Parsed by cxpm's own trusted `JsonValueParser`; never compiled, never `dlopen`'d, never runs as code. This is a direct, structural security advantage of [SRS-json-manifests.md](SRS-json-manifests.md)'s declarative format over the `.cpp` format, independent of anything else in this document. |
| The toolchain's actual compiler/linker/archiver invocations (`Toolchain::object_build`/`executable_link`/...) | Not newly sandboxed here | These already run as separate `popen`'d processes (`Utils::Unix::ShellManager::exec`), not `dlopen`'d in-process — see [SRS-architecture.md](SRS-architecture.md) item A28 for that call site's own (different) hardening needs |

## 3. Requirements

### 3.1 The sandbox helper process (`cxpm-descriptor-sandbox`)

1. A standalone executable, `applications/cxpm-descriptor-sandbox`, SHALL accept exactly two
   arguments: `<project|toolchain>` and `<shared-object-path>`.
2. Before touching the shared object, it SHALL apply, best-effort:
   - `PR_SET_NO_NEW_PRIVS` (Linux; a no-op, not a hard failure, elsewhere);
   - `RLIMIT_CORE = 0` (no core dumps of whatever the loaded code does);
   - `RLIMIT_CPU`, defaulting to 10 seconds, overridable via `CXPM_SANDBOX_CPU_LIMIT_SECONDS`;
   - `RLIMIT_AS`, defaulting to 512 MB, overridable via `CXPM_SANDBOX_MEMORY_LIMIT_MB`;
   - a `SIGALRM`-based wall-clock backstop at `CPU limit + 5` seconds, independent of
     `RLIMIT_CPU` (which only accounts CPU time and would never fire against code blocked on
     I/O rather than spinning). The handler calls `_exit(124)` only — the one call in this file
     required to be async-signal-safe.
3. These constraints SHALL be installed before `dlopen()` is called, so they are already active
   before any of the loaded code's static initializers run.
4. It SHALL `dlopen(path, RTLD_NOW)` the shared object, `dlsym` `get_project`/`get_toolchain`
   (matching the exact getter name the existing generated loader stubs already export — see
   `ProjectManager::BasicProjectLoaderSource` / `ToolchainManager::ToolchainLoaderSource`), call
   it, and print the resulting descriptor as **compact JSON on stdout** via `to_json()` +
   `write_json()` ([SRS-json-manifests.md](SRS-json-manifests.md)).
5. On any failure (missing file, missing symbol, malformed argument, exception during
   serialization), it SHALL print a diagnostic to **stderr** and exit non-zero: `1` for a runtime
   failure, `2` for a usage/argument error. It SHALL NOT print partial/malformed JSON to stdout on
   failure.

### 3.2 The parent-side controller (`CXPM::Controllers::DescriptorSandbox`)

1. `DescriptorSandbox::load_project(shared_object_path) -> ProjectDescriptor` and
   `DescriptorSandbox::load_toolchain(shared_object_path) -> ToolchainDescriptor` SHALL spawn the
   helper, capture its stdout and stderr, wait for it to exit, and on success parse its stdout as
   JSON back into the corresponding descriptor via `*_descriptor_from_json`
   ([SRS-json-manifests.md](SRS-json-manifests.md)). On non-zero exit, they SHALL throw
   `RuntimeException` including the captured stderr text and (if applicable) which signal killed
   the child.
2. The helper SHALL be located, in order: a `CXPM_SANDBOX_HELPER` environment variable override;
   the build tree's own freshly built helper (`cxpm_SANDBOX_HELPER_PATH`, a compile-time macro
   resolved from `$<TARGET_FILE:cxpm-descriptor-sandbox>` — see
   `libraries/cxpm-interface/CMakeLists.txt` — so a project build never depends on the helper
   already being installed system-wide); `<install-prefix>/bin/cxpm-descriptor-sandbox`. If none
   exist, `RuntimeException` SHALL name all three locations checked.
3. The parent SHALL spawn the helper via `posix_spawn()`, not `fork()`. cxpm links a background
   `ThreadPool` singleton (`Core/Threading/ThreadPool.hpp`, used by `ShellManager::exec_async`);
   `fork()`-ing a multithreaded process only duplicates the calling thread, and any lock another
   thread held at fork time stays locked forever in the child — a classic, well-documented hazard.
   `posix_spawn()` sidesteps it entirely.
4. `DescriptorSandbox::enabled()` SHALL return `false` when the environment variable
   `CXPM_SANDBOX_DISABLE` is set to `1`, and `true` otherwise (the default). Both call sites in
   §3.3 SHALL consult this before deciding whether to sandbox or fall back to the pre-sandbox
   direct-`dlopen` behavior.

### 3.3 Call-site integration

1. `ProjectManager::load_from_manifest` SHALL call `DescriptorSandbox::load_project` when
   `DescriptorSandbox::enabled()`, falling back to its original direct `dlopen`/`dlsym` logic
   otherwise. The fallback branch SHALL remain byte-for-byte the pre-sandbox implementation (not
   removed), so `CXPM_SANDBOX_DISABLE=1` is a genuine escape hatch, not merely documentation.
2. `ToolchainManager::load_toolchain_plugin` SHALL follow the identical pattern for
   `DescriptorSandbox::load_toolchain`.
3. Neither call site's public signature or return type SHALL change — sandboxing is an internal
   implementation swap, invisible to `ProjectManager::build_project`/`ToolchainManager::autoscan`
   and everything above them.

## 4. Known limitations (accepted for this iteration)

- **No ABI/schema version check** between the descriptor layout the sandboxed `.cpp` was compiled
  against and the one the parent process expects. This is [SRS-architecture.md](SRS-architecture.md)
  item A23, unchanged by this SRS — the sandbox changes *where* the `dlopen` happens (an isolated
  child instead of the main process), not whether the loaded code's compiled struct layout is
  version-checked against the reader's expectations. Reaching the descriptor through this
  document's JSON IPC does, however, mean a layout mismatch inside the *child* can at worst corrupt
  the child's own memory and produce garbled/rejected JSON (caught by `JsonParseException` in the
  parent) — it can no longer corrupt the *parent* process's memory directly, which is the actual
  security property this SRS is chartered to provide.
- **The generated loader stubs' own type mismatch is untouched.** `BasicProjectLoaderSource`
  declares `extern ProjectDescriptor project;` for a global actually typed `Project` in the
  compiled TU (layout-compatible today because `Project` adds no data members over
  `ProjectDescriptor`, but not enforced by the type system) — pre-existing behavior, documented
  in [SRS-architecture.md](SRS-architecture.md) item A23, not introduced or worsened here.
- **Best-effort, not a hard security boundary.** `setrlimit`/`prctl` failures are not treated as
  fatal (a tightened-but-imperfect sandbox beats none), and nothing here constitutes a full
  container/namespace/seccomp jail. It bounds CPU, memory and wall-clock time and removes direct
  memory access to the parent process; it does not restrict filesystem or network access from
  within the child. A future iteration could add `seccomp-bpf`/Linux namespaces for stronger
  isolation — tracked in [roadmap.md](roadmap.md).
- **One extra process per manifest/toolchain load.** This is a real, measured cost, additive to
  the existing per-invocation JIT-compilation cost documented in
  [SRS-architecture.md](SRS-architecture.md) item A21. It was not separately benchmarked because
  it is dominated by that pre-existing compilation cost (compiling a `toolchain.cpp` takes ~4
  seconds on the reference machine used for [SRS-ctest-tooling.md](SRS-ctest-tooling.md)'s
  integration tests; `posix_spawn`-ing a tiny helper and reading a few hundred bytes from a pipe
  is not the bottleneck). Once item A21's caching is implemented, this cost will be worth
  re-measuring in isolation.

## 5. Testing

| Requirement | Verified by |
|---|---|
| §3.1 helper contract: extracts a correct descriptor, exits 0 | `sandbox_extracts_toolchain_descriptor_as_json` (CTest, `tests/cli/CMakeLists.txt`) — compiles a real `toolchain.cpp` fixture, runs the helper directly against it, asserts the JSON output contains the expected name |
| §3.1 error paths: missing file, wrong kind for the actual symbols present, unknown kind argument | `sandbox_rejects_missing_shared_object`, `sandbox_rejects_mismatched_kind`, `sandbox_rejects_unknown_kind_argument` (CTest) |
| §3.2/§3.3 end to end: a full `cxpm --build` succeeds with the sandbox as the default, active path for both the project-manifest `.so` and the toolchain-plugin `.so` | `cli_build_example_executable` (`.cpp` manifest) and `cli_build_example_executable_json` (`.json` manifest, still exercises the sandboxed toolchain-plugin load) — both pass with `DescriptorSandbox::enabled() == true`, the default |
| `CXPM_SANDBOX_DISABLE=1` fallback still produces a working build | Manually verified during implementation (`cxpm --build examples/executable` with `CXPM_SANDBOX_DISABLE=1` produces the same runnable executable as the sandboxed path); not yet promoted to an automated CTest case — see [roadmap.md](roadmap.md) |

## 6. Acceptance criteria

- `cxpm-descriptor-sandbox toolchain <so>` and `cxpm-descriptor-sandbox project <so>` each print a
  correct, compact-JSON descriptor to stdout and exit 0 for a valid input, and exit non-zero with a
  stderr diagnostic for every error case in §3.1.5.
- `ProjectManager::load_from_manifest` and `ToolchainManager::load_toolchain_plugin` route through
  the sandbox by default, with no change to their return type or the behavior observable by their
  callers.
- The full `cxpm --build`/`cxpm --install` flow, exercised end to end by both integration tests in
  §5, succeeds with sandboxing enabled (the default) exactly as it did before this SRS.
- `CXPM_SANDBOX_DISABLE=1` restores the pre-sandbox direct-`dlopen` behavior for both call sites,
  unchanged.

All of the above hold as of this writing.
