---
title: "SRS: CMake/CTest Tooling"
---

# Software Requirements Specification
## CMake/CTest Command-Execution Tooling Facility

**Project:** cxpm
**Scope:** `tests/CMakeLists.txt`, `tests/cli/CMakeLists.txt`, `cmake/CxpmAddCliTest.cmake`
**Companion documents:** [srs-unit-testing.md](srs-unit-testing.md) (what is tested),
[testing.md](testing.md) (how to run it)

---

## 1. Purpose

cxpm's own product is a command-line build tool: correctness is largely defined by "running
`cxpm <args>` against a project directory produces this exit code / these files / this output".
CTest already is a generic test-execution driver (labels, fixtures, parallelism, timeouts,
pass/fail regex matching) that happens to be bundled with the CMake this project already requires.
This SRS specifies a small, reusable **tooling facility** — a set of CMake functions — that turns
"run this cxpm command and assert on the result" into a declarative one-call registration, so that
CLI-level requirements are checked the same way as unit tests: via `ctest`, with the same
filtering, parallelism and reporting.

## 2. Requirements

### 2.1 Build integration

1. The top-level `CMakeLists.txt` SHALL expose `CXPM_BUILD_TESTS` (default `ON`) gating
   `enable_testing()` and `add_subdirectory(tests)`.
2. The top-level `CMakeLists.txt` SHALL expose `CXPM_BUILD_CLI_INTEGRATION_TESTS` (default `OFF`)
   gating registration of any CTest test that requires a real C++ compiler on `PATH` beyond the
   one used to build `cxpm` itself, so that `ctest` remains fast and hermetic by default.
3. `tests/CMakeLists.txt` SHALL `include(cmake/CxpmAddCliTest.cmake)` and `add_subdirectory(cli)`
   unconditionally (the CLI *smoke* tests, §2.3, do not need
   `CXPM_BUILD_CLI_INTEGRATION_TESTS`); only the heavier integration tests inside `tests/cli`
   SHALL be individually gated by that option.

### 2.2 The tooling facility itself

`cmake/CxpmAddCliTest.cmake` SHALL provide two functions:

#### `cxpm_add_cli_test(<test-name> COMMAND <arg...> [WORKING_DIRECTORY <dir>] [FIXTURES <fixture...>] [LABELS <label...>] [TIMEOUT <seconds>] [WILL_FAIL] [PASS_REGULAR_EXPRESSION <regex>] [FAIL_REGULAR_EXPRESSION <regex>])`

- SHALL register `<test-name>` via `add_test`, running `COMMAND` as a child process — `COMMAND`
  MAY reference generator expressions such as `$<TARGET_FILE:cxpm>` to resolve the just-built
  executable regardless of the generator's output layout.
- SHALL always apply the label `cli` in addition to any caller-supplied `LABELS`.
- SHALL default `TIMEOUT` to 60 seconds when not specified.
- SHALL set `WORKING_DIRECTORY`, `FIXTURES_REQUIRED`, `WILL_FAIL`,
  `PASS_REGULAR_EXPRESSION`/`FAIL_REGULAR_EXPRESSION` test properties when the corresponding
  argument is supplied.

#### `cxpm_add_cli_fixture(<fixture-name> SOURCE_DIRECTORY <dir>)`

- SHALL register a `FIXTURES_SETUP` test that copies `SOURCE_DIRECTORY` into an isolated
  per-fixture directory under the build tree (`${CMAKE_CURRENT_BINARY_DIR}/cli-fixtures/<name>`),
  so a CLI test that mutates its working directory (writing `.o`/`.so`/`compile_commands.json`
  files, as `cxpm --build` does) never mutates the checked-in `examples/` tree.
- SHALL register a matching `FIXTURES_CLEANUP` test that removes the isolated directory.
- SHALL expose the resulting path to the caller as `<fixture-name>_DIRECTORY` in the parent scope.
- A test requiring the fixture SHALL declare it via `cxpm_add_cli_test(... FIXTURES
  <fixture-name> ...)`, which CTest resolves to `FIXTURES_REQUIRED`, guaranteeing the setup test
  runs first and cleanup runs last regardless of `ctest -j` parallelism or `-R` filtering.

### 2.3 Required CLI tests

The following SHALL be registered unconditionally (no compiler beyond the one that built `cxpm`
is required — they exercise argument parsing and early validation, not the build pipeline):

| Test | Command | Expectation |
|---|---|---|
| `cli_help` / `cli_help_short` | `cxpm --help` / `cxpm -h` | exit 0, output matches `Usage:` |
| `cli_no_arguments_prints_usage_and_fails` | `cxpm` | non-zero exit |
| `cli_build_without_directory_fails_cleanly` | `cxpm --build` | non-zero exit (regression for A6/A5, see [srs-architecture.md](srs-architecture.md)) |
| `cli_build_nonexistent_directory_fails_cleanly` | `cxpm --build /nonexistent-...` | non-zero exit |
| `cli_install_without_directory_fails_cleanly` | `cxpm --install` | non-zero exit (regression for A6) |
| `cli_directory_without_package_cpp_fails_cleanly` | `cxpm --build <dir with no package.cpp>` | non-zero exit |

The following SHALL be registered only when `CXPM_BUILD_CLI_INTEGRATION_TESTS=ON`:

| Test | Command | Expectation |
|---|---|---|
| `cli_build_example_executable` | `cxpm --build <fixture copy of examples/executable>`, with `cxpm_BUILD_EXTRA_MODULES_PATH` pointed at the in-tree `modules/toolchains` so no prior `cmake --install` of cxpm is required | exit 0; the example's executable is produced |

### 2.4 Labels

CTest labels SHALL partition the suite along two independent axes so either can be filtered
without the other:

- **What kind of thing is being tested:** `unit` (the `cxpm_tests` binary) vs. `cli` (any test
  invoking the `cxpm` executable as a subprocess).
- **How expensive/hermetic it is:** `smoke` (fast, no compiler dependency) vs. `integration`
  (slow, real compiler required) vs. `fixture` (the setup/cleanup helper tests themselves) vs.
  `regression` (tests that specifically pin a fixed defect, cross-cutting the other labels).

This lets a CI job run `ctest -L unit -L smoke` (or their union) on every push regardless of
compiler availability, while a nightly/slower job additionally runs `ctest -L integration`.

### 2.5 Diagnostics

1. `ctest --output-on-failure` SHALL be the documented default invocation for local development
   ([testing.md](testing.md)), so a failing test's stdout/stderr is visible without a second
   invocation.
2. Every registered test SHALL have a name descriptive enough to identify the failing requirement
   from `ctest`'s summary line alone (see the table in §2.3 — no `test1`/`test2` placeholders).

## 3. Usage examples

```bash
# Configure with tests (default) and the opt-in integration tier
cmake -S . -B build -DCXPM_BUILD_TESTS=ON -DCXPM_BUILD_CLI_INTEGRATION_TESTS=ON
cmake --build build -j

# Everything
ctest --test-dir build --output-on-failure

# Only the hermetic unit suite (no compiler dependency beyond the build itself)
ctest --test-dir build -L unit --output-on-failure

# Only fast CLI argument-parsing/error-path checks
ctest --test-dir build -L smoke --output-on-failure

# Only the regression tests that pin fixed defects
ctest --test-dir build -L regression --output-on-failure

# A single named test
ctest --test-dir build -R cli_build_without_directory_fails_cleanly --output-on-failure

# Parallel run (fixtures still order themselves correctly around it)
ctest --test-dir build -j4 --output-on-failure
```

## 4. Acceptance criteria

- `cmake -S . -B build && cmake --build build -j` succeeds with `CXPM_BUILD_TESTS=ON` (the
  default) and no other options set.
- `ctest --test-dir build` passes in full, including the CLI smoke tier, with no compiler
  requirement beyond the one used for the build itself.
- `ctest --test-dir build -L unit` and `-L smoke` each pass standalone.
- With `-DCXPM_BUILD_CLI_INTEGRATION_TESTS=ON` and a working C++ compiler on `PATH`,
  `ctest --test-dir build -L integration` passes and produces a real, runnable
  `example-executable` under the fixture directory.
- `cxpm_add_cli_test`/`cxpm_add_cli_fixture` are reusable as-is for any future CLI command (e.g. a
  future `--uninstall`, once A13 in [srs-architecture.md](srs-architecture.md) is addressed)
  without modifying `cmake/CxpmAddCliTest.cmake`.

All of the above hold as of this writing.
