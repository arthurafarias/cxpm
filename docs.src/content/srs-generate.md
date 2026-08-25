---
title: "SRS: cxpm --generate"
---

# Software Requirements Specification
## `cxpm --generate`: Scaffolding package.cpp / toolchain.cpp / package.json / toolchain.json

**Project:** cxpm
**Scope:** `CXPM::Modules::Generators::*` (`ManifestGenerator.hpp`), `ApplicationView::generate`
**Companion documents:** [srs-json-manifests.md](srs-json-manifests.md) (the format this feature emits), [tooling.md](tooling.md) (CLI reference)

---

**Note:** §2.1 item 1 below describes `--generate`/`-g` as a flag alongside `-b/--build`/
`-i/--install`. [srs-cli-subcommands.md](srs-cli-subcommands.md) later moved the CLI from flat
flags to git-style subcommands (`cxpm generate <kind> [directory] [--force]`); the *content*
requirements below (§2.2, and §2.1 items 2–8) are otherwise unchanged and still describe current
behavior. This page is left as a dated record rather than rewritten, in the same spirit as
[srs-architecture.md](srs-architecture.md) item A34.

---

## 1. Purpose

Before this feature, starting a new cxpm project meant copying an existing example by hand. This
SRS specifies `cxpm --generate <kind> [directory]`: a command that writes a starter manifest of
the requested kind into a directory, so `cxpm --generate package-json . && cxpm --build .` is a
complete "hello world" workflow with no example project to copy from.

## 2. Requirements

### 2.1 CLI contract

1. `-g|--generate <kind> [directory]` SHALL be recognized alongside the existing
   `-b/--build`/`-i/--install` flags, following the same `ProgramOptions::Parse` convention (the
   flag's captured tokens are `values`; `values.front()` is `kind`, `values[1]`, if present, is
   `directory`).
2. `kind` SHALL be one of exactly four values: `package-cpp`, `package-json`, `toolchain-cpp`,
   `toolchain-json`. Any other value SHALL raise `RuntimeException` naming the four valid kinds.
3. `directory` SHALL default to `.` (the current directory) when omitted.
4. If `directory` does not exist, it SHALL be created (`std::filesystem::create_directories`).
5. The output filename SHALL be fixed per kind: `package.cpp`, `package.json`, `toolchain.cpp`,
   `toolchain.json` respectively — matching exactly what `assert_project_directory`
   ([srs-json-manifests.md](srs-json-manifests.md)) and `ToolchainManager::autoscan` look for, so
   the freshly generated file is immediately usable with no rename.
6. If the target file already exists, `--generate` SHALL refuse to overwrite it and raise
   `RuntimeException`, **unless** `--force` is also passed.
7. On success, the command SHALL log the written path (via `LoggerManager::info`) and return exit
   code 0; any failure (unknown kind, refused overwrite, filesystem error) SHALL propagate as an
   exception, caught by `main()`'s existing handler, resulting in a non-zero exit code — consistent
   with every other command's exit-code contract (see [srs-architecture.md](srs-architecture.md)
   item A7, which established that contract for `--install`).
8. `print_usage()`'s help text SHALL document `--generate`, its four kinds, and `--force`.

### 2.2 Content requirements

1. **`package-cpp`** and **`toolchain-cpp`** SHALL be rendered from a fixed C++ source template
   (via `std::format`) using the `Target`/`Project` and `Toolchain` fluent builder APIs
   respectively — i.e. the generated source SHALL be idiomatic, using the same builder pattern
   documented in [tooling.md](tooling.md), not the broken `TargetDescriptor()`/`ProjectDescriptor()`
   pattern [srs-architecture.md](srs-architecture.md) item A30 fixed in the README.
2. **`package-json`** and **`toolchain-json`** SHALL NOT be hand-written string templates. They
   SHALL be produced by constructing a real `Target`/`ProjectDescriptor` or `Toolchain` object with
   representative example values, then serializing it through the same `to_json()` +
   `write_json_pretty()` path defined in [srs-json-manifests.md](srs-json-manifests.md). This
   guarantees, by construction, that a generated `package.json`/`toolchain.json` is always
   parseable by `ProjectManager::load_project`/`ToolchainManager::autoscan` — there is no separate
   "generator's idea of the schema" that can drift from "the parser's idea of the schema", because
   both sides go through the identical descriptor structs.
3. The generated target/toolchain `name` field SHALL default to the basename of `directory` (with
   fallback `"example"` for an empty basename, e.g. `directory == "."`), so
   `cxpm --generate package-json my-tool` produces a target literally named `my-tool` without
   further editing.
4. JSON output SHALL be pretty-printed (`write_json_pretty`), since generated files are meant to
   be hand-edited next, unlike the compact JSON `ProjectManager`/`ToolchainManager` read back at
   build time.

## 3. Non-goals

- No interactive prompts (target type, dependencies, etc.) — `--generate` produces one opinionated
  starting point per kind; further customization is ordinary manifest editing.
- No project-wide scaffolding (a `src/` directory with a populated `main.cpp`, a `.gitignore`,
  etc.) in this iteration. `--generate` only ever writes the one manifest file it was asked for.
- `--generate toolchain-*` does not attempt to detect a real compiler on the host and pre-fill
  `compiler_executable`/`linker_executable`/`archiver_executable` with discovered paths; it uses
  `/usr/bin/c++`/`/usr/bin/ar` as a starting point, consistent with how the shipped
  `modules/toolchains/{gcc,g++,nvcc}` plugins hardcode their own executable paths today.

## 4. Testing

| Requirement | Verified by |
|---|---|
| §2.2 content correctness (templates embed the right name, JSON round-trips through the real descriptor parser) | `ManifestGeneratorTest` (`CXPM/Testing/ManifestGeneratorTest.hpp`) — pure unit tests, no subprocess needed, since the generator functions are plain `String`-returning functions |
| §2.1 CLI contract (file written, overwrite protection, `--force`, unknown-kind rejection) | CTest CLI tests in `tests/cli/CMakeLists.txt`: `cli_generate_package_json_writes_file`, `cli_generate_refuses_to_overwrite_without_force`, `cli_generate_overwrites_with_force`, `cli_generate_unknown_kind_fails_cleanly` |
| End-to-end: a generated manifest actually builds | Manually verified during implementation (`cxpm --generate package-json` followed by `cxpm --build` on the result produces a running executable); the equivalent is exercised continuously by `cli_build_example_executable_json` against the checked-in `examples/executable-json/package.json`, which has the same shape as generator output |

## 5. Acceptance criteria

- All four `kind` values produce a file that is immediately valid input to the corresponding
  consumer (`package-cpp`/`package.json` → `cxpm --build`; `toolchain-cpp`/`toolchain.json` →
  `ToolchainManager::autoscan`), with no manual editing required.
- Re-running `--generate` against an existing file fails cleanly without `--force` and succeeds
  with it.
- `cxpm --generate` with no `kind` argument, or an unrecognized `kind`, fails cleanly with a
  message naming the valid options, matching every other command's "fail fast with a clear
  message" contract established across [srs-architecture.md](srs-architecture.md)'s regression
  fixes.

All of the above hold as of this writing.
