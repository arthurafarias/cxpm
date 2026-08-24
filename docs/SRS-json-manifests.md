---
layout: default
title: "SRS: JSON Manifests"
---

# Software Requirements Specification
## package.json / toolchain.json as Serialization Alternatives

**Project:** cxpm
**Scope:** `libraries/cxpm-interface/src/CXPM/Modules/Serialization/{JsonValueParser,JsonValueWriter,JsonManifest,JsonInputArchiver}.hpp`, `ProjectManager::load_project`, `ToolchainManager::autoscan`, `ApplicationView::assert_project_directory`
**Companion documents:** [architecture.md](architecture.md), [SRS-generate.md](SRS-generate.md) (produces these files), [SRS-sandbox.md](SRS-sandbox.md) (uses the same codec as its IPC format)

---

## 1. Purpose

cxpm's manifests (`package.cpp`, `toolchain.cpp`) are C++ translation units: expressive, but they
require compiling and `dlopen`-ing a fresh shared object on every single invocation (see
[SRS-architecture.md](SRS-architecture.md) item A21), and they are inherently code — a
`package.cpp` can contain arbitrary static-initializer logic, not just declarative configuration.

This SRS specifies a **declarative, serialization-only alternative**: `package.json` and
`toolchain.json`, parsed directly into the exact same `ProjectDescriptor`/`ToolchainDescriptor`
structs the `.cpp` path produces, with no compilation and no `dlopen` involved. Both formats
describe the same manifest; a project or a toolchain author picks whichever is a better fit, and
the two are read through the same internal representation, so nothing downstream of "the manifest
has been loaded" needs to know or care which format was on disk.

## 2. Requirements

### 2.1 Codec (the serializers this SRS is built on)

1. `CXPM::Core::Containers::Value` (the existing Variant-based JSON-like tree — `null`, `bool`,
   `int`, `double`, `String`, `BasicCollection<Value>`, `Map<String, Value>`) SHALL be
   round-trippable to and from JSON text.
2. `parse_json(const String&) -> Value` (`JsonValueParser.hpp`) SHALL implement a
   recursive-descent JSON parser supporting all seven `Value` alternatives, standard string
   escapes (`\" \\ \/ \b \f \n \r \t`), `\uXXXX` escapes for Basic-Multilingual-Plane codepoints,
   and both integer and floating-point number literals (an exponent or a decimal point selects
   `double`; otherwise `int`). It SHALL throw `CXPM::Core::Exceptions::JsonParseException` (a
   `RuntimeException` subclass) with a descriptive message on any malformed input, including
   trailing content after a complete value.
3. `write_json(const Value&) -> String` (`JsonValueWriter.hpp`) SHALL render a `Value` tree as
   compact JSON text, using `std::quoted`-style string escaping consistent with the existing
   `JsonOutputArchiver`.
4. `write_json_pretty(const Value&, indent_width=2) -> String` SHALL render the same tree with
   newlines and indentation, for human-authored/human-edited output (used by
   [SRS-generate.md](SRS-generate.md)); it SHALL NOT be required to match `write_json`'s exact
   whitespace, only to parse back to an equal `Value` tree.
5. `AbstractInputArchiver`'s constructor SHALL be public (it was previously placed before the
   class's first access specifier, making it unconstructible — see
   [SRS-architecture.md](SRS-architecture.md) item A33). `JsonInputArchiver` SHALL wrap an
   `std::istream`, read it to completion at construction time, and expose the parsed `Value` via
   `.value()`.

### 2.2 Descriptor-level conversion

`JsonManifest.hpp` SHALL provide, for `TargetDescriptor`, `ToolchainDescriptor` and
`ProjectDescriptor`:

1. `to_json(const T&) -> Value`, covering every field of `T` (excluding `compile_commands`/
   `compile_comands`, which are build *output*, not manifest input, on both `TargetDescriptor`
   and `ProjectDescriptor`).
2. `T_from_json(const Value&) -> T` (named `target_descriptor_from_json`,
   `toolchain_descriptor_from_json`, `project_descriptor_from_json`), which SHALL:
   - throw `JsonParseException` if the input `Value` is not a JSON object;
   - throw `JsonParseException` if a present field has the wrong JSON type;
   - fall back to `T`'s own documented default for any field absent from the input (e.g. a
     `TargetDescriptor` with no `"type"` key SHALL still default to `"executable"`, matching
     `TargetDescriptor.hpp`'s in-class default member initializer) — a JSON manifest is never
     required to spell out every field, exactly like a `Target()` builder chain that only calls
     the setters it needs.
3. `ProjectDescriptor`'s conversion SHALL recurse into `to_json`/`*_from_json` for each element of
   its `targets`/`toolchains` collections.

### 2.3 Manifest discovery and precedence

1. `ProjectManager::load_project(project_path, manifest_toolchain, out_compile_commands)` SHALL
   replace the previous hardcoded "always compile `package.cpp`" logic in `build_project`, and
   SHALL:
   - use the `package.cpp` path (compile + `dlopen`, exactly as before) when `package.cpp` exists
     in `project_path`;
   - otherwise use the `package.json` path (read + `parse_json` + `project_descriptor_from_json`,
     no compilation) when `package.json` exists;
   - otherwise throw `RuntimeException`.
2. **`package.cpp` takes precedence over `package.json`** when both exist in the same directory,
   so adding a `package.json` next to an existing `package.cpp` (e.g. as a generated
   human-readable summary) is never a silent behavior change for an existing project.
3. `ToolchainManager::autoscan`'s directory walk SHALL match both `toolchain.cpp` (existing:
   compile + `dlopen`) and `toolchain.json` (new: read + parse, no compilation) filenames.
   **`toolchain.cpp` takes precedence over a sibling `toolchain.json` in the same directory**, for
   the same reason as 2.2 above; a `toolchain.json` is skipped by `autoscan` whenever
   `toolchain.cpp` exists alongside it.
4. `ApplicationView::assert_project_directory` SHALL accept a directory containing either
   `package.cpp` or `package.json` (previously it required `package.cpp` specifically, so
   `cxpm --build <dir-with-only-package.json>` would fail the pre-flight check before ever
   reaching `ProjectManager`).
5. Toolchain discovery (`ToolchainManager::autoscan`, via `ToolchainManager::current`) SHALL run
   exactly once per `build_project` invocation regardless of which manifest format is in use.
   (This requirement exists because of a bug found and fixed while implementing this SRS: toolchain
   discovery previously only happened as a side effect of `build_manifest` calling
   `ToolchainManager::current` to pick a toolchain for compiling `package.cpp` itself — a
   `package.json` manifest never triggered it at all, leaving the toolchain registry empty for
   `autoselect`/`by_name` later in `build_project`. `build_project` now calls
   `ToolchainManager::current` explicitly, once, up front, and threads the result into
   `load_project`/`build_manifest` so `autoscan`'s toolchain JIT-compilation is not paid twice on
   the `package.cpp` path either.)

## 3. Non-goals

- **No fully symmetric token-driven input archiver.** `JsonOutputArchiver`'s
  `ObjectStartToken`/`PairToken`/... DSL is a good fit for *writing* (the writer controls emission
  order), but mirroring it for *reading* would require matching a fixed token sequence against
  arbitrary input structure with lookahead — meaningfully more code for no behavioral benefit over
  parsing to a `Value` tree first and converting. `JsonInputArchiver` therefore parses eagerly at
  construction rather than exposing a token-by-token pull interface.
- **No schema validation beyond field-level type checking.** An unknown key in a `package.json`
  object is silently ignored (matching how an unrecognized method call would simply fail to
  compile in the `.cpp` path — there is no equivalent "did you typo a field name" diagnostic for
  JSON). A future version could warn on unrecognized keys.
- **JSON does not, and will not, execute code.** This is a deliberate property, not a gap: a
  `package.json`/`toolchain.json` cannot contain arbitrary logic the way a `.cpp` file can, and
  therefore never needs the isolation [SRS-sandbox.md](SRS-sandbox.md) provides for the compiled
  path — see that document's §5 for the direct comparison.

## 4. Testing

| Requirement | Verified by |
|---|---|
| §2.1.2–2.1.4 (parse/write round trip, escapes, pretty-printing, malformed input) | `JsonValueCodecTest` (`CXPM/Testing/JsonValueCodecTest.hpp`) |
| §2.1.5 (`JsonInputArchiver`) | `JsonManifestTest`: "JsonInputArchiver reads an istream into a Value tree" |
| §2.2 (descriptor conversion, defaults, malformed-field rejection) | `JsonManifestTest` (`CXPM/Testing/JsonManifestTest.hpp`) — round-trips `TargetDescriptor`, `ToolchainDescriptor`, a multi-target/multi-toolchain `ProjectDescriptor`, and asserts `JsonParseException` on type mismatches |
| §2.3 (manifest discovery, precedence, one-time toolchain scan) | `examples/executable-json` built end to end by the opt-in `cli_build_example_executable_json` CTest integration test (see [SRS-ctest-tooling.md](SRS-ctest-tooling.md)), which exercises the real `cxpm` binary, real toolchain discovery, and produces a runnable executable from a `package.json` alone — no `package.cpp`, no manifest compilation, no manifest `dlopen` |

## 5. Acceptance criteria

- `parse_json(write_json(v)) == v` (by field-for-field comparison) for every `Value` shape
  exercised by `JsonValueCodecTest`.
- A `TargetDescriptor`/`ToolchainDescriptor`/`ProjectDescriptor` round-trips through
  `to_json`/`*_from_json` with no data loss for every field populated by `JsonManifestTest`.
- `cxpm --build examples/executable-json` (no `package.cpp` present) builds and produces a
  runnable executable, with no compiler invocation for the manifest itself (verified manually:
  the build log for this example contains no `package.cpp`/`package.loader.cpp` compilation step,
  only the toolchain plugin's).
- `cxpm --build <dir>` behaves identically whether `<dir>` has `package.cpp` or `package.json`
  from the caller's perspective (same exit-code contract, same `compile_commands.json` output for
  the sources actually built), modulo the manifest-compilation step itself not existing on the
  JSON path.

All of the above hold as of this writing.
