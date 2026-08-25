---
title: "SRS: Architecture & Flaws"
---

# Software Requirements Specification
## Current Architecture, Itemized Requirements and Corrective Flaws

**Project:** cxpm
**Method:** this SRS was produced by reading the entire `cxpm-interface` public API
(every header under `libraries/cxpm-interface/src/CXPM/`), `applications/cxpm/src/cxpm.cpp`, and
all three toolchain plugins, then itemizing the architecture as it actually behaves today —
not as aspirational design.
**Companion documents:** [architecture.md](architecture.md) (component diagram),
[srs-unit-testing.md](srs-unit-testing.md) (test coverage this SRS's requirements imply)

---

## 1. Purpose

cxpm had no written specification of its own current behavior. This document does two things:

1. States, as numbered requirements, what the system **actually does today** (§3) — a
   requirements-shaped description of the as-built architecture, so future changes can be
   evaluated against a known baseline instead of tribal knowledge.
2. Itemizes every concrete, cited defect found while establishing that baseline (§4), with a
   severity, a suggested correction, and — for the subset fixed as part of this change — the
   regression test that pins the fix.

## 2. Architecture overview

See [architecture.md](architecture.md) for the full component table and control-flow diagram.
In one paragraph: `cxpm` (`Views::ApplicationView`) parses `argv` into build/install commands;
`Controllers::ProjectManager` compiles the target directory's `package.cpp` into a shared object
and `dlopen`s it back to recover a `Project`; `Controllers::ToolchainManager` discovers
`toolchain.cpp` plugins on a search path, JIT-compiles each into a shared object, `dlopen`s it
once to copy out a `ToolchainDescriptor`, then immediately `dlclose`s it; `Toolchain` (the sole
`ToolchainInterface` implementation) turns that descriptor plus a `TargetDescriptor` into shell
command lines executed through `Utils::Unix::ShellManager::exec` (`popen`-based).

## 3. Requirements derived from current (as-built) behavior

These are descriptive, not aspirational — each is cited against the code that implements it.

- **AR-1.** The system SHALL accept a project description as a compiled C++ translation unit
  (`package.cpp`) rather than a parsed configuration format (`Controllers::ProjectManager`,
  `ProjectManager.hpp`).
- **AR-2.** The system SHALL derive toolchain identity from a `toolchain.cpp` plugin that defines
  a global `CXPM::ToolchainDescriptor toolchain` object and exports it through a generated
  `extern "C" const ToolchainDescriptor* get_toolchain()` loader stub
  (`ToolchainManager.hpp:180-185`, `233-262`).
- **AR-3.** Toolchain and project manifests SHALL be recompiled from source and `dlopen`'d on
  every invocation; no persistent artifact cache SHALL be consulted
  (`ToolchainManager::autoscan`, `ToolchainManager.hpp:122-177`).
- **AR-4.** Compiler/linker/archiver invocation SHALL be constructed as a shell command line and
  executed via `popen` (`Utils::Unix::ShellManager::exec`, `ShellManager.hpp`), not via a
  structured process-spawn API.
- **AR-5.** The public builder types (`Target`, `Project`, `Toolchain`) SHALL expose a fluent,
  chainable interface returning `DerivedType&` from every setter, matching the CRTP interfaces in
  `TargetInterface.hpp`/`ToolchainDescriptorInterface.hpp`.
- **AR-6.** Discovery of toolchain plugins SHALL search, in order: a `cxpm_BUILD_EXTRA_MODULES_PATH`
  environment variable, then a fixed list of system/user install-prefix-relative directories
  (`ProjectManager::initialize`, `ProjectManager.hpp:50-62`).
- **AR-7.** Failure to discover, compile, or load a resource (compiler missing, compile error,
  `dlopen`/`dlsym` failure, missing `package.cpp`) SHALL raise
  `CXPM::Core::Exceptions::RuntimeException` with a descriptive message, not an error code.
- **AR-8.** Installation SHALL copy already-built artifacts into a POSIX-aligned prefix layout
  (`lib`, `bin`, `include/<project>`, `lib/pkgconfig`) via `ProjectManager::install_target`
  (`ProjectManager.hpp:193-311`); it SHALL NOT invoke `Toolchain::install`.

## 4. Itemized flaws

Severity: **Critical** (UB/crash/silently wrong result reachable from normal CLI use), **High**
(incorrect behavior or dead API surface with no workaround), **Medium** (maintainability/latent
risk, not user-visible today), **Low** (cosmetic/naming).

Status: **Fixed** = corrected and regression-tested as part of this change (see
[srs-unit-testing.md §6](srs-unit-testing.md#6-regression-test-requirement)). **Deferred** =
itemized here with a suggested correction but intentionally not changed in this pass, either
because the fix is a larger design decision (see [roadmap.md](roadmap.md)) or because it touches
code with no current test coverage that would catch a regression.

### 4.1 Fixed in this change

| ID | Area | Description | Evidence | Correction applied |
|---|---|---|---|---|
| **A1** | `String` | `join` called `.front()` on a possibly-empty `BasicCollection<String>` — UB on an empty list | `String.hpp:27` (pre-fix) | Added an `empty()` guard returning `String()` |
| **A2** | `BasicCollection` | The JSON array `operator%` called `.front()` unconditionally before checking whether the collection was empty — UB on any empty `sources`/`dependencies`/`toolchains` list, i.e. a completely normal state | `BasicCollection.hpp:49` (pre-fix) | Guarded the `front()`/loop with `if (!collection.empty())` |
| **A3** | `String` | `trim`/`trim_left`/`trim_right` tested `std::isprint`, which treats space as printable, so they never actually stripped leading/trailing whitespace | `String.hpp:45,58` (pre-fix); found by writing `StringTest` | Switched to `std::isspace(static_cast<unsigned char>(...))` |
| **A4** | `String` | `trim_right`'s `if (end == 0) break;` safeguard left one leading whitespace character un-trimmed on an all-whitespace string; `end` is a signed `int`, so the safeguard was solving a non-existent unsigned-underflow problem | `String.hpp:64-65` (pre-fix) | Removed the safeguard; the existing `end >= 0` loop condition already terminates correctly at `end == -1` |
| **A5** | `ProgramOptions::Parse` | A flag token that is the last token, or immediately followed by another flag, is never inserted into the result map (e.g. `Parse({"--help"})` has no `"help"` key) | `Parse.hpp:16-24` (pre-fix) | Insert `options[current_argument]` eagerly when a flag token is seen, not only when a following value token arrives |
| **A6** | `ApplicationView` | `--install` read `values.front()` with no emptiness check, unlike the sibling `--build` path which explicitly checks `values.empty()` first | `ApplicationView.hpp:81` (pre-fix) | Added the same `values.empty()` check and `assert_project_directory` call `--build` already had |
| **A7** | `ApplicationView` | `run()`'s install path had a `switch (status) { case Failure: case Success: break; }` — both arms were no-ops, so a failed install still exited `0` | `ApplicationView.hpp:83-87` (pre-fix) | `case Status::Failure` now `return 1;` |
| **A8** | `PkgConfigManager` | `emit_configuration` substituted `package.name` into the `{{version}}` placeholder (copy-paste from the line above) and `toolchain.install_prefix` into `{{description}}` | `PkgConfigManager.hpp:66-69` (pre-fix) | `{{version}}` now takes `package.version`; `{{description}}` takes an empty string, since `PackageDescriptor` has no description field to substitute (see A9, deferred) |
| **A30** | Documentation | `readme.md`'s primary example used `TargetDescriptor()`/`ProjectDescriptor()` with builder methods (`.name_set`, `.create()`, `.add()`) that only exist on `Target`/`Project` | `readme.md:22-36` (pre-fix) | Rewrote the example to use `Target`/`Project`, matching every `examples/*/package.cpp` and the README's own later "Custom Toolchain Support" section |
| **A31** | `Target` | `examples/executable-with-gstreamer-1.0/package.cpp` called `.dependencies_append(...)`, which didn't exist on `Target`/`TargetInterface` (only `dependencies_set`/`dependencies_get`, unlike `sources`/`options`/`include_directories`/`link_directories`/`link_libraries`, which all have `*_append`) | `Target.hpp` (pre-fix); example `package.cpp` | Added `Target::dependencies_append(const String&)` for parity with every other collection field, pinned by `TargetTest`: "dependencies_append accumulates across multiple calls" |
| **A34** | `ProjectManager` | `build_project`'s error message referenced `"package.json"` (`.../package.json` as the second `{}` in `"Failed to build manifest {} using {}"`) while the only manifest mechanism at the time was a compiled `package.cpp` — leftover text from an earlier design iteration, and confusing on its own terms even before this session | `ProjectManager.hpp:103` (pre-fix) | Superseded by `ProjectManager::load_project` ([srs-json-manifests.md](srs-json-manifests.md)), whose own error messages correctly reference `package.cpp`/`package.json` as the two *real* manifest mechanisms — this finding is now moot rather than separately patched |
| **A37** | `ProjectManager`/`ToolchainManager` | Toolchain discovery (`ToolchainManager::autoscan`) only ran as a side effect of `build_manifest` calling `ToolchainManager::current` to pick a toolchain for compiling `package.cpp` itself — an entirely incidental trigger. Invisible before this session because there was only one manifest code path; became a real bug the moment [srs-json-manifests.md](srs-json-manifests.md)'s `package.json` path was added (confirmed by reproduction: `cxpm --build` against a `package.json`-only directory failed with "Couldn't find a compatible toolchain" because the toolchain registry was never populated) | `ProjectManager.hpp` `build_project`/`build_manifest` (pre-fix) | `build_project` now calls `ToolchainManager::current` explicitly, exactly once, up front, regardless of manifest format, and threads the result into `load_project`/`build_manifest` so `autoscan`'s JIT-compilation is not paid twice on the `package.cpp` path either |
| **A16** | `Core::{SharedPointer,UniquePointer,WeakPointer}`, `Utils/Print.hpp`, `ProgramOptions::OptionDescriptor` | Thin wrapper/utility headers never used anywhere in the codebase | grep-confirmed dead | `ProgramOptions::OptionDescriptor`/`OptionsDescriptorCollection` are now the live schema `CommandLineParser` validates every subcommand's options against and `HelpFormatter` renders usage text from — see [srs-cli-subcommands.md](srs-cli-subcommands.md). `Core::{SharedPointer,UniquePointer,WeakPointer}` and `Utils/Print.hpp` remain unused and are re-itemized below as A16b |
| **A35** | `ApplicationView` | `OptionsDescriptorCollection` is constructed in `run()` and then never used — `print_usage()` is a hardcoded string instead of being generated from the schema | `ApplicationView.hpp:30-31` (pre-fix) | `run()` now builds a `CommandRegistry` of `CommandDescriptor`s (each carrying its own `OptionsDescriptorCollection`) and dispatches against it; `print_usage()`/the new per-command help both call `HelpFormatter::{top_level,command}`, which render directly from that registry — see [srs-cli-subcommands.md](srs-cli-subcommands.md) |
| **A17** | Namespacing | Two incompatible `Controllers` namespaces existed: `CXPM::Controllers` (`ProjectManager`, `ToolchainManager`) and a separate global-scope `::Controllers` (`PkgConfigManager`, `ClangdManager`); `Toolchain.hpp:618` resolved `Controllers::PackageConfigManager` via unqualified lookup reaching the global one only by luck of C++ lookup rules. This stopped being latent and became a real, reproduced build failure while implementing [srs-sandbox.md](srs-sandbox.md): adding `CXPM::Controllers::DescriptorSandbox` shifted which `Controllers` unqualified lookup found first from inside `namespace CXPM`, and `Toolchain.hpp:618` broke outright with "`CXPM::Controllers::PackageConfigManager` has not been declared" | `PkgConfigManager.hpp`, `ClangdManager.hpp` (pre-fix) | Consolidated `PkgConfigManager.hpp` and `ClangdManager.hpp` onto `namespace CXPM::Controllers`; updated the one call site that had spelled it out as `::Controllers::PackageConfigManager` (`PkgConfigManagerTest.hpp`) to `CXPM::Controllers::PackageConfigManager` |

### 4.2 Deferred (itemized, not changed in this pass)

| ID | Area | Description | Evidence | Severity | Suggested correction |
|---|---|---|---|---|---|
| **A9** | `PkgConfigManager` | `emit_configuration` takes a `PackageDescriptor`, which has no `description` field, so there is structurally no data to fill `{{description}}` with even after A8 | `PkgConfigManager.hpp:52-54`, `PackageDescriptor.hpp` | Medium | Change the signature to accept a `TargetDescriptor` (which does have `description`) or add an explicit `description` parameter |
| **A10** | `Toolchain::install` | Both overloads unconditionally `throw NotImplementedException()`; nothing calls them because real install logic lives entirely in `ProjectManager::install_target`'s hand-rolled file copying | `Toolchain.hpp:662-672` | High | Either implement `Toolchain::install` and route the CLI through it, unifying the two divergent install code paths, or remove `install()` from `ToolchainInstallInterface` |
| **A11** | `ProjectManager::install_project` | Always `return {Status::Failure}`, ignoring its arguments; never called — `ApplicationView` has its own same-named, different, actually-used private method | `ProjectManager.hpp:184-188` | Medium | Delete the dead stub, or make `ApplicationView::install_project` delegate to it |
| **A12** | `ProjectManager::build_target` | A pass-through stub that always reports `Status::Success` from a fresh, unresolved, default-constructed `Toolchain`, without building anything | `ProjectManager.hpp:175-180` | High | Implement for real (select toolchain, call `toolchain.build(target)`) or remove it |
| **A13** | `ApplicationView` | A stub private `uninstall()` method exists, but `run()` never dispatches to it and it is not registered as a command — entirely unreachable from the CLI. (Previously it was at least *documented* in `print_usage()` despite doing nothing; [srs-cli-subcommands.md](srs-cli-subcommands.md) deliberately left it out of the new `CommandRegistry` rather than list a command that does nothing — see that SRS's non-goals.) | `ApplicationView.hpp` (`uninstall` method) | Medium | Implement it and register a real `uninstall` command, or delete the stub until it is |
| **A14** | `ClangdManager`, `ShellCommandBuilder`, `KeyValueMapDescriptor` | Three headers are never `#include`d anywhere and, if they were, would fail to compile — all three misuse the non-template `Collection` struct as if it were `template<typename T> Collection<T>` | `ClangdManager.hpp:28`, `ShellCommandBuilder.hpp:28,36`, `Modules/Serialization/KeyValueMapDescriptor.hpp:11` | Medium | Either make `Collection` a real alias template (`template<typename T> using Collection = BasicCollection<T>;`) matching these call sites, or fix the call sites to use `BasicCollection<T>` directly and finish or delete the three orphan headers |
| **A15** | `Utils/Unix/which.hpp` | Completely empty file; `which()` is actually implemented inside `EnvironmentManager.hpp` instead | `Utils/Unix/which.hpp` | Low | Move `which` into this file, or delete it |
| **A16b** | `Core::{SharedPointer,UniquePointer,WeakPointer}`, `Utils/Print.hpp` | Thin wrapper/utility headers never used anywhere in the codebase (renumbered from A16, whose third item, `ProgramOptions::OptionDescriptor`, is now fixed — see §4.1) | grep-confirmed dead | Low | Start using them for a stated reason, or remove them to shrink the API surface a test suite has to cover |
| **A18** | Namespacing | `Core::Containers::Tuple` and `Modules::Serialization::KeyValueMapDescriptor` are declared without the `CXPM::` prefix every sibling header uses, making them unreachable under their intended fully-qualified name | `Core/Containers/Tuple.hpp:8` | Low | Add the missing `CXPM::` prefix, or delete if genuinely unused |
| **A19** | `ToolchainManager` | `by_name`/`autoselect`/`current` (and several `LoggerManager` stream setters) are marked `constexpr` despite throwing exceptions, doing filesystem I/O, or touching mutable static/atomic state at runtime — ill-formed, no diagnostic required, and misleading to readers | `ToolchainManager.hpp:61-93,99-120`, `LoggerManager.hpp:76,80,89,93,112,116,120` | Low | Drop `constexpr`, keep `inline`/`static` |
| **A20** | `ToolchainManager::autoselect` | Two unconditional `std::cout <<` debug lines, inconsistent with the rest of the codebase which routes diagnostics through `LoggerManager` | `ToolchainManager.hpp:80,90` | Low | Replace with `LoggerManager::debug(...)` |
| **A21** | `ToolchainManager::autoscan` | Recompiles every discovered `toolchain.cpp` into a fresh `.so` on **every** invocation, with no mtime/hash cache — confirmed empirically: a single `cxpm --build` recompiles all three toolchains (~4s each on this machine) before doing any real work | `ToolchainManager.hpp:122-177`, C.2 of the codebase audit | High (performance) | Cache by source mtime/hash; only recompile when the plugin source changed |
| **A22** | `ToolchainManager` plugin naming | The generated `.so`'s logical name is derived from the *parent directory* of `toolchain.cpp` (`source_path.parent_path().filename()`), not from the descriptor's own `.name_set(...)` value; in the source tree this makes all three plugins (`gcc/src/`, `g++/src/`, `nvcc/src/`) resolve to the same directory-derived name `"src"`, so they overwrite each other's generated `.loader.cpp`/`.so` files mid-scan — confirmed working today only because each plugin is fully compiled, loaded and `dlclose`'d before the next one starts | `ToolchainManager.hpp:191`; confirmed by running `cxpm --build` with `cxpm_BUILD_EXTRA_MODULES_PATH` pointed at `modules/toolchains` | Medium | Derive the generated filename from the descriptor's `.name_get()` after a first `dlopen`, or require plugins to sit directly under `<name>/toolchain.cpp` (matching the *installed* layout, which does not have this collision) |
| **A23** | `ToolchainManager`/`ProjectManager` `dlopen` | No ABI/schema version check between the `ToolchainDescriptor`/`Project` layout a plugin was compiled against and the one `cxpm` itself uses; `ProjectManager`'s loader `reinterpret_cast`s a `Project*` getter from a stub that actually returns `const ProjectDescriptor*` | `ProjectManager.hpp:364-388,454-460`, `ToolchainManager.hpp:233-262` | Medium | Add an explicit format/ABI version field checked before trusting the loaded pointer |
| **A24** | `BasicCollection` | Privately inherits `Lockable` (dragging in a mutex) but never calls `acquire_lock()` from any of its own operations — looks thread-aware, provides zero synchronization | `Core/Containers/BasicCollection.hpp:18-19` | Medium | Either remove the unused `Lockable` base, or actually synchronize `transform`/mutating operations if concurrent access is a real requirement |
| **A25** | `Lockable` | Every copy gets a **new** `std::make_shared<std::mutex>()` rather than sharing the original — two "copies" of the same logical collection do not protect each other | `Core/Patterns/Lockable.hpp:8-15` | Medium | Decide whether `Lockable` should have copy-shares-lock or copy-gets-fresh-lock semantics, and document/enforce whichever is intended |
| **A26** | `EnvironmentManager::which` | Uses `recursive_directory_iterator` over every `PATH` entry (not direct children), never checks the executable bit, and the dedicated `which.hpp` file meant to hold it is empty (A15) | `Utils/Unix/EnvironmentManager.hpp:64-71` | Medium | Iterate only direct children of each `PATH` directory and check `X_OK` |
| **A27** | `EnvironmentManager::get` | Uses non-reentrant `strtok` (the codebase spins up threads via `ThreadPool`) and splits on both `;` and `:` in a `CXPM::Utils::Unix`-namespaced (POSIX-only) class | `Utils/Unix/EnvironmentManager.hpp:34-43` | Low | Use `std::string::find`/`substr` splitting instead of `strtok`; split on `:` only |
| **A28** | `ShellManager::exec` | Command construction is injection-prone: `shell=true` wraps as `bash -c "<cmd>"` with no escaping, and `popen` itself always runs through `/bin/sh -c`, producing a double shell-wrap; target names/paths and pkg-config output flow unescaped into these command lines | `Utils/Unix/ShellManager.hpp:38-39,45`, `Toolchain.hpp:327,419,517,570`, `PkgConfigManager.hpp:24` | High (security) | Prefer `posix_spawn`/`fork`+`exec` with an argv array over `popen`+string concatenation; if a shell is genuinely needed, quote every interpolated argument |
| **A29** | Toolchain deployment | `build.sh` compiles the three toolchains to prebuilt `.so` files and copies them to `/usr/share/cxpm/toolchains`, but `ToolchainManager::autoscan` only ever looks for files literally named `toolchain.cpp` — the two deployment mechanisms are mutually incompatible and `build.sh`'s `.so` output is silently ignored | `build.sh`, `ToolchainManager.hpp:157-163` | Medium | Pick one deployment mechanism; if prebuilt `.so` is preferred (recommended given A21's JIT-recompilation cost), rewrite `autoscan` to discover and version-check `.so` files directly |
| **A32** | `ProjectDescriptor::compile_comands` | Misspelled field (missing the second "m"), inconsistent with the correctly-spelled `TargetDescriptor::compile_commands` and `CompileCommandDescriptor` | `ProjectDescriptor.hpp:15` | Low | Rename to `compile_commands` |
| **A33** | `AbstractInputArchiver` | Constructor is `private` (declared before the `protected:` label), making the class unconstructible as written; no input archiver (JSON or otherwise) exists anywhere — deserialization is entirely unimplemented | `Modules/Serialization/AbstractInputArchiver.hpp:9-19` | Medium | Fix the access specifier; decide whether manifest deserialization is in scope, and if not, remove the interface until it is |
| **A36** | CI | No CI configuration exists anywhere in the repository (`readme.md`'s git history references a past "github-actions compiler" change, but no workflow file survives in the current tree) | repo-wide search | High (process) | Add a CI workflow that runs `ctest -L unit` (and, on a runner with a compiler, the full suite) on every push/PR — see [roadmap.md](roadmap.md) |

## 5. Requirements for tests covering correct function

For every **Fixed** item in §4.1, [srs-unit-testing.md §6](srs-unit-testing.md#6-regression-test-requirement)
names the exact test case pinning it — that table is the traceability link this SRS's "cover in
tests the requirements to function correctly" mandate is checked against. For **Deferred** items,
the correction column above doubles as the acceptance criterion a future fix must satisfy, and
each deferred item SHOULD gain its own regression test in the same PR that fixes it, following the
pattern established in §4.1.

## 6. Acceptance criteria

- Every item in §4.1 has landed and is pinned by a named test (verified: see
  [srs-unit-testing.md](srs-unit-testing.md)).
- Every item in §4.2 is recorded with a severity and a concrete suggested correction, so it can be
  picked up independently without re-deriving the analysis (this document).
- No item in §4.2 is silently contradicted by §3's as-built requirements — where a deferred item
  changes intended behavior (e.g. A21, A29), §3 is written to describe *current* behavior, not the
  corrected target, so the two sections never conflict about what "correct" means today.
