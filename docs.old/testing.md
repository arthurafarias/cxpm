---
layout: default
title: Testing
---

# Testing

cxpm's test suite has two tiers: an in-process **unit** binary (`cxpm_tests`) and a set of
**CLI** tests that run the real `cxpm` executable (and the `cxpm-descriptor-sandbox` helper) as
subprocesses through CTest. See [SRS-unit-testing.md](SRS-unit-testing.md) for the requirements
this suite satisfies and [SRS-ctest-tooling.md](SRS-ctest-tooling.md) for the CMake tooling that
registers the CLI tier.

## Running the suite

```bash
cmake -S . -B build -DCXPM_BUILD_TESTS=ON
cmake --build build -j

ctest --test-dir build --output-on-failure
```

```text
Test project build
 1/13 Test  #1: cxpm_tests .......................................   Passed
 2/13 Test  #2: cli_help .........................................   Passed
 3/13 Test  #3: cli_help_short ...................................   Passed
 4/13 Test  #4: cli_no_arguments_prints_usage_and_fails ..........   Passed
 5/13 Test  #5: cli_build_without_directory_fails_cleanly ........   Passed
 6/13 Test  #6: cli_build_nonexistent_directory_fails_cleanly ....   Passed
 7/13 Test  #7: cli_install_without_directory_fails_cleanly ......   Passed
 8/13 Test  #8: cli_directory_without_package_cpp_fails_cleanly ..   Passed
 9/13 Test  #9: fixture:generate:clean ...........................   Passed
10/13 Test #10: cli_generate_package_json_writes_file ............   Passed
11/13 Test #11: cli_generate_refuses_to_overwrite_without_force ..   Passed
12/13 Test #12: cli_generate_overwrites_with_force ................  Passed
13/13 Test #13: cli_generate_unknown_kind_fails_cleanly ...........  Passed

100% tests passed out of 13 (with -DCXPM_BUILD_CLI_INTEGRATION_TESTS=OFF, the default)
```

`cxpm_tests` itself reports 70 passing unit-test cases across 16 self-registering groups.

Enable the slower, real-compiler-dependent integration tests too — this additionally covers
building `examples/executable` (`package.cpp`) and `examples/executable-json` (`package.json`,
see [SRS-json-manifests.md](SRS-json-manifests.md)) end to end, plus the
`cxpm-descriptor-sandbox` helper directly (see [SRS-sandbox.md](SRS-sandbox.md)):

```bash
cmake -S . -B build -DCXPM_BUILD_CLI_INTEGRATION_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

With integration tests enabled, `ctest --test-dir build` registers 23 tests (1 unit binary + 22
CLI-tier tests, counting fixture setup/cleanup) and passes in full, repeatably — every test that
writes output cleans up after itself or is guarded by a `FIXTURES_SETUP` step, so running `ctest`
twice in a row in the same, uncleaned build directory passes both times.

## Per-class self-registering groups

Every class or free function with meaningful behavior carries its own test group, colocated with
the code it covers: `<Subject>Test.hpp` lives in a `Testing/` subdirectory next to the header it
tests, in that header's namespace with a nested `::Testing` sub-namespace appended. The harness
itself, which isn't a test of any single namespaced class, stays under the top-level
`libraries/cxpm-interface/src/CXPM/Testing/`:

```text
libraries/cxpm-interface/src/CXPM/Testing/TestFailure.hpp                          # harness
libraries/cxpm-interface/src/CXPM/Testing/TestContext.hpp                          # harness: check/equal/throws
libraries/cxpm-interface/src/CXPM/Testing/TestCase.hpp                             # harness
libraries/cxpm-interface/src/CXPM/Testing/TestGroup.hpp                            # harness: self-registration
libraries/cxpm-interface/src/CXPM/Testing/Registry.hpp                             # harness
libraries/cxpm-interface/src/CXPM/Testing/RunAll.hpp                               # harness: entry point
libraries/cxpm-interface/src/CXPM/Testing/TemporaryDirectory.hpp                   # harness: fs fixture helper
libraries/cxpm-interface/src/CXPM/Core/Containers/Testing/StringTest.hpp
libraries/cxpm-interface/src/CXPM/Core/Containers/Testing/BasicCollectionTest.hpp
libraries/cxpm-interface/src/CXPM/Core/Containers/Testing/ObjectTest.hpp
libraries/cxpm-interface/src/CXPM/Core/Containers/Testing/ValueTest.hpp
libraries/cxpm-interface/src/CXPM/Core/Exceptions/Testing/ExceptionsTest.hpp
libraries/cxpm-interface/src/CXPM/Core/Functional/Testing/FunctionTest.hpp
libraries/cxpm-interface/src/CXPM/Core/Logging/Testing/LoggerManagerTest.hpp
libraries/cxpm-interface/src/CXPM/Core/Patterns/Testing/LockableTest.hpp
libraries/cxpm-interface/src/CXPM/Core/Threading/Testing/ThreadPoolTest.hpp
libraries/cxpm-interface/src/CXPM/Modules/Generators/Testing/ManifestGeneratorTest.hpp # SRS-generate.md
libraries/cxpm-interface/src/CXPM/Modules/ProgramOptions/Testing/ParseTest.hpp
libraries/cxpm-interface/src/CXPM/Modules/Serialization/Testing/JsonOutputArchiverTest.hpp
libraries/cxpm-interface/src/CXPM/Modules/Serialization/Testing/JsonValueCodecTest.hpp # SRS-json-manifests.md
libraries/cxpm-interface/src/CXPM/Modules/Serialization/Testing/JsonManifestTest.hpp   # SRS-json-manifests.md
libraries/cxpm-interface/src/CXPM/Utils/Unix/Testing/EnvironmentManagerTest.hpp
libraries/cxpm-interface/src/CXPM/Utils/Unix/Testing/ShellManagerTest.hpp
libraries/cxpm-interface/src/CXPM/Testing/StatusTest.hpp
libraries/cxpm-interface/src/CXPM/Testing/TargetTest.hpp
libraries/cxpm-interface/src/CXPM/Testing/ProjectTest.hpp
libraries/cxpm-interface/src/CXPM/Testing/ToolchainTest.hpp
libraries/cxpm-interface/src/CXPM/Testing/ToolchainDescriptorTest.hpp
libraries/cxpm-interface/src/CXPM/Testing/CompilerCommandDescriptorTest.hpp
libraries/cxpm-interface/src/CXPM/Testing/PkgConfigManagerTest.hpp         # CXPM::Controllers::Testing
libraries/cxpm-interface/src/CXPM/Testing/ProjectManagerTest.hpp          # CXPM::Controllers::Testing
libraries/cxpm-interface/src/CXPM/Testing/ToolchainManagerTest.hpp        # CXPM::Controllers::Testing
```

`PkgConfigManager`, `ProjectManager` and `ToolchainManager` declare `namespace CXPM::Controllers`
even though their headers live directly under `CXPM/` rather than a `CXPM/Controllers/` folder;
their tests stay colocated with those headers (in `CXPM/Testing/`) and use the class's actual
namespace, `CXPM::Controllers::Testing`, rather than a folder-derived one. `ThreadPool` has the
same kind of mismatch (`CXPM/Core/Threading/` but `namespace CXPM::Threading`) — its test uses
`CXPM::Threading::Testing`.

Each test header declares a struct named after the class it covers, deriving from `TestGroup`
(from `CXPM::Testing`), inside its own namespace's nested `::Testing`:

```cpp
// libraries/cxpm-interface/src/CXPM/Core/Containers/Testing/StringTest.hpp
namespace CXPM::Core::Containers::Testing {

struct StringTest : public TestGroup {
  StringTest() : TestGroup("String", {
    {"join concatenates with delimiter", [](TestContext& ctx) {
      BasicCollection<String> parts{"a", "b", "c"};
      ctx.equal(String::join(parts, "-"), String("a-b-c"));
    }},
  }) {}
};

inline static StringTest string_test_instance;

} // namespace CXPM::Core::Containers::Testing
```

Constructing `string_test_instance` at static-init time runs `TestGroup`'s constructor, which
self-registers the group into `CXPM::Testing::registry()`. `tests/CMakeLists.txt` globs every
`*Test.hpp` header under any `Testing/` directory and generates one source file that includes all
of them, so every test suite coexists in one translation unit — exactly the property that surfaces
accidental helper-name collisions and ODR problems at compile time, mirrored from how `opheap`'s
newer test architecture works.

Because tests live under a `Testing/` subdirectory (the top-level harness, or one colocated with
each namespace), `cxpm-interface`'s `install(TARGETS ... FILE_SET HEADERS ...)` excludes every
such subdirectory (see `libraries/cxpm-interface/CMakeLists.txt`), so installed packages ship
production headers only.

## Testability tiers

Not every component can be unit-tested the same way; the suite's structure follows directly from
this classification (see [SRS-unit-testing.md §2.2/§8](SRS-unit-testing.md) for the requirements
this implies):

**Easy — pure/deterministic, no I/O:** `String`, `BasicCollection<T>::transform`, `Value`/`Variant`,
`ProgramOptions::Parse`, `Status`, `RuntimeException`/`NotImplementedException`,
`Target`/`Project`/`ToolchainDescriptor` builder chains, `JsonOutputArchiver` + tokens,
`CompileCommandDescriptor`'s `operator%`, `PackageConfigManager::emit_configuration`, the
`Value`↔JSON codec and descriptor-level `to_json`/`*_from_json` conversions (see
[SRS-json-manifests.md](SRS-json-manifests.md)), and the `--generate` template functions (see
[SRS-generate.md](SRS-generate.md)) — all pure `String`-in/`String`-out functions.

**Medium — a subprocess but no compiler/filesystem-heavy setup:** the `cxpm` executable's
argument-parsing and early-validation exit codes (the CLI *smoke* tier), including `--generate`'s
overwrite-protection behavior.

**Hard — real compiler, `dlopen`, populated filesystem:** `ToolchainManager` (discovery, JIT
compilation, loading), `ProjectManager::build_project`/`install_target`, `Toolchain`'s real
`object_build`/`executable_link`/`shared_object_link`/`archive_link` exec paths,
`EnvironmentManager::which`, `PackageConfigManager::find_package`, and the
`cxpm-descriptor-sandbox` helper (see [SRS-sandbox.md](SRS-sandbox.md)), which needs a real
compiled shared object to load. These are covered only by the opt-in CLI *integration* tier
(`CXPM_BUILD_CLI_INTEGRATION_TESTS=ON`), which runs a real `cxpm --build` against fixture copies
of `examples/executable` and `examples/executable-json`, and the sandbox helper directly against
a freshly compiled toolchain-plugin fixture.

## Correctness classes currently exercised

- string `join`/`trim`/`trim_left`/`trim_right`/`split`, including empty-input and
  all-whitespace edge cases;
- `BasicCollection<T>::transform` over empty and non-empty collections;
- every `Value`/`Variant` alternative, including the wrong-alternative `std::get` failure mode;
- `Status`'s `std::to_string` specialization;
- `RuntimeException`'s `std::format`-based message construction and its exception hierarchy;
- `ProgramOptions::Parse`'s long/short-flag, positional, and trailing/valueless-flag handling;
- the full fluent builder surface of `Target` and `Toolchain`/`ToolchainDescriptor`, and
  `Project`'s target/toolchain composition;
- `JsonOutputArchiver`'s token-by-token rendering, including empty-collection array serialization;
- `CompileCommandDescriptor`'s JSON `operator%`;
- `PackageConfigManager::emit_configuration`'s `.pc`-template substitution;
- the generic `Value` ↔ JSON text codec: every alternative, string escapes including `\u` BMP
  codepoints, nested structures, pretty-printing, and malformed-input rejection;
- `TargetDescriptor`/`ToolchainDescriptor`/`ProjectDescriptor` ↔ JSON round trips, including
  default-field fallback for a partial JSON object and rejection of a wrong-typed field;
- all four `cxpm --generate` template kinds, including that the generated `package.json` parses
  back into the exact `ProjectDescriptor` shape `ProjectManager::load_project` expects;
- the `cxpm` executable's `--help`/no-argument/missing-directory/nonexistent-directory/
  missing-manifest/`--generate` exit-code contract, including overwrite protection and `--force`;
- the `cxpm-descriptor-sandbox` helper's success and every documented failure path (missing file,
  symbol/kind mismatch, unknown kind argument) run directly, independent of a full build;
- (opt-in) a full `cxpm --build` against real example projects in both manifest formats, through
  real toolchain discovery, JIT compilation, the sandboxed descriptor-loading path, and compiler
  invocation.

## Sanitizers

`-DCXPM_ENABLE_SANITIZERS=ON` (the default) builds `cxpm_tests` with
`-fsanitize=address,undefined` on non-MSVC compilers. The full suite passes clean under both.

## Coverage instrumentation

`-DCXPM_ENABLE_COVERAGE=ON` builds `cxpm_tests` with `--coverage` and, if `gcovr` is on `PATH`,
adds a `coverage` target that runs the unit suite and regenerates an HTML report under
`docs/coverage`:

```sh
cmake -S . -B build-coverage -DCXPM_BUILD_TESTS=ON -DCXPM_ENABLE_COVERAGE=ON
cmake --build build-coverage --target coverage
```

The report counts only `libraries/cxpm-interface/src/`, excluding `CXPM/Testing/` itself, since
the goal is coverage of the code under test, not of the test harness.

## Known gaps

See [SRS-unit-testing.md §8](SRS-unit-testing.md#8-deferred-coverage--future-work) for the
itemized list of components not yet unit-tested (`ToolchainManager`, `ProjectManager`'s
orchestration logic, `ShellManager` fault injection, `EnvironmentManager::which`/`get`) and why.
`CXPM_SANDBOX_DISABLE=1`'s fallback path (see [SRS-sandbox.md §5](SRS-sandbox.md#5-testing)) is
verified manually but not yet an automated CTest case.
