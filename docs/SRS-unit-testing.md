---
layout: default
title: "SRS: Unit Testing the API"
---

# Software Requirements Specification
## Unit Testing the cxpm Public API

**Project:** cxpm
**Language:** C++23, header-only interface library (`cxpm-interface`)
**Scope of this document:** every header under `libraries/cxpm-interface/src/CXPM/`, the `cxpm`
CLI entry point (`applications/cxpm/src/cxpm.cpp` + `CXPM::Views::ApplicationView`), and the
three toolchain plugins (`modules/toolchains/{gcc,g++,nvcc}`)
**Status at the time of writing:** implemented — see [testing.md](testing.md) for the current
pass/fail state and how to reproduce it.

---

## 1. Purpose

Before this document, cxpm had no test infrastructure of any kind: no `tests/` directory, no
`enable_testing()`, no CI. This SRS defines the requirements for a unit-test suite that covers
the entire public API surface of `cxpm-interface` and the argument-parsing/exit-code contract of
the `cxpm` executable, and it records the concrete suite that was implemented to satisfy those
requirements.

## 2. Scope

### 2.1 In scope

- Every class, free function and template documented in [architecture.md](architecture.md)'s
  component table.
- The fluent builder contracts of `Target`, `Project`, `Toolchain`/`ToolchainDescriptor`.
- The pure, deterministic containers under `CXPM::Core::Containers` (`String`, `BasicCollection`,
  `Collection`, `Value`, `Variant`, `Map`, `Object`).
- The JSON serialization DSL under `CXPM::Modules::Serialization` (tokens + `JsonOutputArchiver`).
- `CXPM::Modules::ProgramOptions::Parse`.
- `CXPM::Core::Exceptions::{RuntimeException, NotImplementedException}`.
- `CXPM::Status` and its `std::to_string` specialization.
- `Controllers::PackageConfigManager::emit_configuration` (the pure half of `PkgConfigManager`).
- The `cxpm` executable's argument-parsing and exit-code contract, exercised as a black box
  through CTest (see [SRS-ctest-tooling.md](SRS-ctest-tooling.md)).

### 2.2 Out of scope for this iteration

Per the testability assessment below, the following require a real compiler, `dlopen`, or a
populated filesystem and are covered only at the CLI/integration tier, not as pure unit tests:

- `Controllers::ToolchainManager` (`autoscan`, `build_toolchain_plugin`, `load_toolchain_plugin`).
- `Controllers::ProjectManager::build_project`/`build_manifest`/`install_target`.
- `Toolchain::object_build`/`executable_link`/`shared_object_link`/`archive_link` and their
  `_async` variants (they shell out via `Utils::Unix::ShellManager::exec`).
- `Utils::Unix::EnvironmentManager::which`/`get` (real filesystem/environment).
- `Controllers::PackageConfigManager::find_package` (shells out to `pkg-config`).

These are documented as deferred work in [§8](#8-deferred-coverage--future-work), not silently
dropped.

## 3. Test tiers

| Tier | What it exercises | Dependencies | CTest label |
|---|---|---|---|
| **Unit** | Pure/deterministic classes and functions, in-process | none (no filesystem, no subprocess, no compiler) | `unit` |
| **CLI smoke** | `cxpm`'s argument parsing and exit codes, as a subprocess | the already-built `cxpm` binary only | `smoke`, `cli` |
| **CLI integration** (opt-in) | A full `cxpm --build` against a real example project | a working C++ compiler on `PATH`; `CXPM_BUILD_CLI_INTEGRATION_TESTS=ON` | `integration`, `cli` |

This split exists because roughly a third of the public API (toolchain discovery, JIT plugin
compilation, process execution) is inherently an integration concern — see
[testing.md §Testability tiers](testing.md#testability-tiers) for the full component-by-component
classification this split is derived from.

## 4. Test-harness requirements

1. Tests SHALL be colocated with the code they cover, under
   `libraries/cxpm-interface/src/CXPM/Testing/`, following the project's existing convention of
   one class per header (mirrors `CXPM/Core/Containers/String.hpp` ↔ a would-be
   `CXPM/Testing/StringTest.hpp`, matching how `opheap`'s per-library `testing/*_test.hpp` trees
   are organized).
2. Each test file SHALL declare exactly one `CXPM::Testing::TestGroup`-derived struct, named
   `<Subject>Test`, and instantiate one `inline static` instance of it so that constructing the
   translation unit's static objects self-registers the group into
   `CXPM::Testing::registry()` — no test runner needs to enumerate test files by hand.
3. `CXPM::Testing::TestContext` SHALL provide at minimum `check(bool, message)`,
   `equal(actual, expected, message)`, and `throws<ExceptionType>(callable, message)`.
4. The `Testing/` subtree SHALL be excluded from `cxpm-interface`'s installed `FILE_SET HEADERS`
   (`libraries/cxpm-interface/CMakeLists.txt`), so consumers of the installed package never see
   test-only headers.
5. CMake SHALL discover every `CXPM/Testing/*Test.hpp` header via `file(GLOB_RECURSE ...
   CONFIGURE_DEPENDS ...)` and generate one aggregate translation unit that `#include`s all of
   them, exactly as `tests/CMakeLists.txt` does — this surfaces ODR violations and name
   collisions across test files at compile time instead of hiding them behind independent binaries.
6. The aggregate binary (`cxpm_tests`) SHALL be registered with `add_test(NAME cxpm_tests ...)`
   and labeled `unit`.

## 5. Coverage requirements by component

Each row is a requirement ("the suite SHALL exercise …") together with the test group that
satisfies it today.

| Component | Header | Required coverage | Test group | Status |
|---|---|---|---|---|
| `String` | `Core/Containers/String.hpp` | `join` (multi/single/**empty**), `trim`/`trim_left`/`trim_right` (including all-whitespace and empty-string edge cases), `split` (with and without matches) | `StringTest` | Implemented |
| `BasicCollection<T>` | `Core/Containers/BasicCollection.hpp` | `transform` (non-empty and empty), inherited `std::deque` operations | `BasicCollectionTest` | Implemented |
| `Value`/`Variant` | `Core/Containers/Value.hpp`, `Variant.hpp` | every alternative constructible and `std::get`-able; `std::get` on the wrong alternative throws `std::bad_variant_access` | `ValueTest` | Implemented |
| `Status` | `Status.hpp` | `std::to_string` for every enumerator | `StatusTest` | Implemented |
| `RuntimeException`/`NotImplementedException` | `Core/Exceptions/*.hpp` | message formatting, exception-hierarchy catchability | `ExceptionsTest` | Implemented |
| `ProgramOptions::Parse` | `Modules/ProgramOptions/Parse.hpp` | long/short flags with values, positional arguments, **trailing/valueless flags** | `ParseTest` | Implemented (regression, see [SRS-architecture.md](SRS-architecture.md)) |
| `Target`/`TargetDescriptor` | `Target.hpp`, `TargetDescriptor.hpp` | every fluent setter/getter pair round-trips, `*_append` accumulates across calls, `create()` identity, documented defaults | `TargetTest` | Implemented |
| `Project`/`ProjectDescriptor` | `Project.hpp`, `ProjectDescriptor.hpp` | `add(TargetDescriptor)`, `add(ToolchainDescriptor)`, multi-add composition, `create()` identity | `ProjectTest` | Implemented |
| `ToolchainDescriptor`/`Toolchain` (descriptor half) | `ToolchainDescriptor.hpp`, `Toolchain.hpp` | documented default prefixes/suffixes, fluent setter/getter round trip | `ToolchainDescriptorTest` | Implemented |
| `CompileCommandDescriptor` | `CompilerCommandDescriptor.hpp` | `operator%` JSON serialization of every field | `CompilerCommandDescriptorTest` | Implemented |
| `JsonOutputArchiver` + tokens | `Modules/Serialization/*.hpp` | bool/null/int32/String/const-char-array tokens, `PairToken`, object/array/separator structural tokens, **empty `BasicCollection<String>` array serialization** | `JsonOutputArchiverTest` | Implemented (regression) |
| `PackageConfigManager::emit_configuration` | `PkgConfigManager.hpp` | name/prefix/version substitution into the `.pc` template | `PkgConfigManagerTest` | Implemented (regression) |
| `Value` ↔ JSON codec (`parse_json`/`write_json`/`write_json_pretty`) | `Modules/Serialization/JsonValue{Parser,Writer}.hpp` | every alternative, string escapes incl. `\u` BMP, nested structures, pretty-printing, malformed-input rejection — see [SRS-json-manifests.md](SRS-json-manifests.md) | `JsonValueCodecTest` | Implemented |
| `to_json`/`*_descriptor_from_json` | `Modules/Serialization/JsonManifest.hpp` | round trip for `TargetDescriptor`/`ToolchainDescriptor`/`ProjectDescriptor`, default-field fallback, wrong-type rejection — see [SRS-json-manifests.md](SRS-json-manifests.md) | `JsonManifestTest` | Implemented |
| `JsonInputArchiver` | `Modules/Serialization/JsonInputArchiver.hpp` | reads an `istream` into a `Value` tree | `JsonManifestTest` | Implemented |
| `ManifestGenerator` (`--generate` templates) | `Modules/Generators/ManifestGenerator.hpp` | all four kinds; JSON kinds round-trip through the real descriptor parser — see [SRS-generate.md](SRS-generate.md) | `ManifestGeneratorTest` | Implemented |
| `cxpm` argument parsing & exit codes | `Views/ApplicationView.hpp` (via the built binary) | `--help`/`-h`, no-arguments, `--build`/`--install`/`--generate` with no/invalid arguments, nonexistent directory, directory without a manifest, `--generate` overwrite protection | `tests/cli/CMakeLists.txt` CTest tests | Implemented (CLI tier) |
| `cxpm-descriptor-sandbox` helper | `applications/cxpm-descriptor-sandbox/src/main.cpp` | success path and every documented failure path, run directly — see [SRS-sandbox.md](SRS-sandbox.md) | `tests/cli/CMakeLists.txt` CTest tests (`sandbox_*`) | Implemented (CLI/integration tier) |
| `ToolchainManager` (discovery/JIT/loading) | `ToolchainManager.hpp` | plugin discovery, compilation, sandboxed loading, validation | — | **Deferred** (needs a real compiler; covered indirectly by the opt-in `cli_build_example_executable`/`cli_build_example_executable_json` integration tests) |
| `ProjectManager` (build/install orchestration) | `ProjectManager.hpp` | `build_project`, `install_target`, manifest round trip for both formats | — | **Deferred**, same reason |
| `Utils::Unix::{EnvironmentManager,ShellManager}` | `Utils/Unix/*.hpp` | `which`, `get`, `exec`/`exec_async` | — | **Deferred**, needs filesystem/process fixtures — see [§8](#8-deferred-coverage--future-work) |

70 unit-test cases across 16 test groups pass today; see [testing.md](testing.md) for the live
count and how to reproduce it locally.

## 6. Regression-test requirement

Every defect fixed as part of implementing this SRS SHALL have a test that fails against the
pre-fix behavior and passes against the fix, so the defect cannot silently reappear. The following
regressions are pinned:

| Defect | Fixed in | Pinned by |
|---|---|---|
| `String::join` UB on an empty collection | `Core/Containers/String.hpp` | `StringTest`: "join of an empty collection returns an empty string instead of invoking undefined behavior" |
| `BasicCollection` JSON array serialization UB on an empty collection | `Core/Containers/BasicCollection.hpp` | `JsonOutputArchiverTest`: "an empty BasicCollection\<String\> serializes as an empty JSON array …" |
| `String::trim*` used `std::isprint` instead of `std::isspace`, so it never actually stripped spaces | `Core/Containers/String.hpp` | `StringTest`: "trim removes leading and trailing non-printable characters", "trim_right on an all-whitespace string returns an empty string without underflowing" |
| `String::trim_right`'s `if (end == 0) break;` safeguard left one leading character un-trimmed on an all-whitespace string | `Core/Containers/String.hpp` | same as above |
| `ProgramOptions::Parse` dropped a trailing or immediately-re-flagged valueless flag | `Modules/ProgramOptions/Parse.hpp` | `ParseTest`: "a valueless flag that is the last token …", "… immediately followed by another flag …" |
| `PackageConfigManager::emit_configuration` substituted `package.name` into `{{version}}` instead of `package.version` | `PkgConfigManager.hpp` | `PkgConfigManagerTest`: "substitutes name, prefix and version …" |
| `cxpm --install` with no directory read `values.front()` on a possibly-empty collection (UB), unlike the sibling `--build` path | `Views/ApplicationView.hpp` | `tests/cli`: `cli_install_without_directory_fails_cleanly` |
| `cxpm --install`'s exit code never reflected `Status::Failure` (`switch` with two no-op `break`s) | `Views/ApplicationView.hpp` | exercised by the CLI tier; see [SRS-architecture.md §4](SRS-architecture.md#4-itemized-flaws) item A2 |

The `String::trim*` bug (isprint vs. isspace, and the `trim_right` off-by-one) was **not** in the
original defect list — it was found by writing the tests this SRS requires and watching them fail
against the pre-fix implementation. That is the intended value of §5: writing coverage for a
"boring", supposedly-already-correct function is exactly how latent bugs like this surface.

## 7. Build & CI requirements

1. `CXPM_BUILD_TESTS` (default `ON`) SHALL gate `enable_testing()` and `add_subdirectory(tests)`
   from the top-level `CMakeLists.txt`.
2. `CXPM_ENABLE_SANITIZERS` (default `ON`) SHALL build `cxpm_tests` with
   `-fsanitize=address,undefined` on non-MSVC compilers.
3. `CXPM_ENABLE_COVERAGE` (default `OFF`, opt-in) SHALL build `cxpm_tests` with `--coverage` and,
   when `gcovr` is available, expose a `coverage` CMake target that regenerates
   `docs/coverage/index.html`, restricted to `libraries/cxpm-interface/src/` and excluding
   `Testing/`.
4. `ctest` (no arguments) SHALL run the full suite (unit + CLI smoke + any opt-in integration
   tests) and exit non-zero on any failure.
5. `ctest -L unit` SHALL run only the hermetic unit suite, suitable for every CI run regardless of
   whether a C++ compiler is available on `PATH` beyond the one that built `cxpm_tests` itself.

## 8. Deferred coverage & future work

- **`ToolchainManager`/`ProjectManager` unit-level coverage.** Both are static-only "controller"
  classes with private mutable static state and no dependency-injection seam (see
  [SRS-architecture.md](SRS-architecture.md) item B29). Unit-testing them in isolation would
  require either exposing a `reset()`/injection hook or refactoring them into instantiable objects
  that accept a `ToolchainInterface`/filesystem abstraction. Until then they are covered only at
  the CLI/integration tier via `cli_build_example_executable`.
- **`ShellManager::exec`/`exec_async` fault injection.** Unlike `opheap`'s
  `fault_backend`/`fault_file`/`fault_plan` harness for storage faults, cxpm has no equivalent for
  simulating a failing/partial compiler invocation. A fake `ToolchainInterface` implementation
  (the interfaces are already pure-virtual and mockable, see
  [architecture.md](architecture.md)) would let `ProjectManager`'s orchestration logic be
  unit-tested without a real compiler.
- **`EnvironmentManager::which`/`get`.** Needs a temp-directory-based `PATH` fixture
  (`CXPM::Testing::temporary_directory`, already implemented in
  `CXPM/Testing/TemporaryDirectory.hpp` for this purpose) plus `setenv`/`unsetenv` around each
  test case.
- **`Map<K,V>`'s JSON `operator%`.** Calls `.front()` on a `std::map`, which has no such member —
  this is a hard compile error if ever instantiated, and is therefore untested and untestable
  until it is redesigned (see [SRS-architecture.md](SRS-architecture.md) item B1/Map).

## 9. Acceptance criteria

This SRS is satisfied when:

- every component listed in [§5](#5-coverage-requirements-by-component) as "Implemented" has a
  passing, self-registering `TestGroup`;
- `ctest -L unit` passes with zero failures on a clean checkout;
- `cxpm_tests` builds and passes under `-fsanitize=address,undefined`;
- every regression in [§6](#6-regression-test-requirement) is pinned by a named test case;
- the CLI smoke tier passes without requiring a working C++ compiler beyond the one used to build
  `cxpm` itself.

All of the above hold as of this writing — see [testing.md](testing.md) for the reproducible
`ctest` output.
