---
title: Architecture
---

# Architecture

## Control flow

```text
argv
  │
  ▼
Views::ApplicationView::run()
  │  parses --help/--build/--install/--generate via Modules::ProgramOptions::Parse
  │
  ├── --build <dir> ──────────────┐         ├── --generate <kind> [dir] ─────┐
  ├── --install <dir> [--prefix] ─┤         ▼                                ▼
  ▼                               ▼   Modules::Generators::ManifestGenerator (§ srs-generate.md)
Controllers::ProjectManager       Controllers::ProjectManager    writes package.cpp / package.json /
  build_project(dir)                install_target(...)          toolchain.cpp / toolchain.json
  │                                  │  filesystem copy into      (the *.json kinds are rendered
  │                                  │  <prefix>/{bin,lib,...}     through to_json()+write_json_pretty(),
  ▼                                  ▼                             not a hand-written string template)
load_project(dir): package.cpp   (built artifacts must already
present? compile it + a           exist on disk — install does
generated loader stub into        not itself build)
libproject-manifest.so;
package.json present? parse
it directly — no compiler,
no shared object, no dlopen
  │
  │  a compiled package.cpp's descriptor is extracted via
  ▼  Controllers::DescriptorSandbox (§ srs-sandbox.md), not a direct dlopen
Controllers::ToolchainManager::current(target)
  │  autoscan(): walk search paths for toolchain.cpp OR toolchain.json
  │  toolchain.cpp: JIT-compile into libtoolchain-<name>.so, then
  │                 DescriptorSandbox::load_toolchain() extracts the descriptor
  │  toolchain.json: parse directly — no compiler, no shared object, no dlopen
  ▼
Toolchain (the sole ToolchainInterface implementation)
  │  builds compiler/linker/archiver command lines
  ▼
Utils::Unix::ShellManager::exec (popen)
  │
  ▼
real compiler/linker/archiver process
```

`DescriptorSandbox` (`CXPM::Controllers::DescriptorSandbox`, backed by the
`cxpm-descriptor-sandbox` helper executable) `posix_spawn`s a disposable, resource-limited child
process to `dlopen` a compiled `package.cpp`/`toolchain.cpp` shared object and hand its descriptor
back as JSON over a pipe, rather than `dlopen`-ing that (potentially third-party) compiled code
directly inside the running `cxpm` process. See [srs-sandbox.md](srs-sandbox.md).

Every JIT-compilation step above is uncached: toolchain plugins are recompiled from source and
loaded fresh on every single `cxpm --build`/`--install` invocation, including the project
manifest itself, whenever the `.cpp` manifest format is used. See
[srs-architecture.md](srs-architecture.md) item A21 for the performance implication and a
suggested caching correction — and see [srs-json-manifests.md](srs-json-manifests.md) for the
`.json` alternative, which pays neither this compilation cost nor needs the sandbox at all, since
it never executes code in the first place.

## Components

| Component | Namespace | Responsibility |
|---|---|---|
| `ApplicationView` | `CXPM::Views` | Parses `argv`, dispatches to build/install/generate, owns the CLI's exit-code contract |
| `ProjectManager` | `CXPM::Controllers` | Loads `package.cpp`/`package.json` manifests (`load_project`); orchestrates per-target build and install |
| `ToolchainManager` | `CXPM::Controllers` | Discovers, loads (compiling `toolchain.cpp` or parsing `toolchain.json`) toolchain descriptors |
| `DescriptorSandbox` | `CXPM::Controllers` | `posix_spawn`s `cxpm-descriptor-sandbox` to extract a descriptor from a compiled manifest/toolchain `.so` out of process — see [srs-sandbox.md](srs-sandbox.md) |
| `Toolchain` | `CXPM` | The one concrete `ToolchainInterface`: turns a `TargetDescriptor` + `ToolchainDescriptor` into shell command lines for object/executable/shared-object/archive builds |
| `PackageConfigManager` | `CXPM::Controllers` | Resolves `pkg-config` dependencies; renders a minimal `.pc` template |
| `Target`/`TargetDescriptor` | `CXPM` | Fluent builder / POD pair describing one build target |
| `Project`/`ProjectDescriptor` | `CXPM` | Fluent builder / POD pair aggregating targets and toolchains |
| `ToolchainDescriptor` | `CXPM` | POD describing one toolchain's executables, options and file-naming conventions |
| `String`, `BasicCollection<T>`, `Map<K,V>`, `Value`/`Variant` | `CXPM::Core::Containers` | Hand-rolled STL-adjacent value types used throughout the public API instead of raw `std::string`/`std::vector`/`std::variant` |
| `JsonOutputArchiver` + tokens | `CXPM::Modules::Serialization` | A `%`-operator streaming DSL used to render `compile_commands.json` |
| `JsonValueParser`/`write_json`/`write_json_pretty` | `CXPM::Modules::Serialization` | The `Value` ↔ JSON text codec backing `package.json`/`toolchain.json` and `DescriptorSandbox`'s IPC format — see [srs-json-manifests.md](srs-json-manifests.md) |
| `to_json`/`*_descriptor_from_json` (`JsonManifest.hpp`) | `CXPM::Modules::Serialization` | Descriptor ↔ `Value` conversion for `TargetDescriptor`/`ToolchainDescriptor`/`ProjectDescriptor` |
| `ManifestGenerator` | `CXPM::Modules::Generators` | Backs `cxpm --generate`; the `*-json` kinds are rendered through the serializers above, not hand-written — see [srs-generate.md](srs-generate.md) |
| `ShellManager` | `CXPM::Utils::Unix` | `popen`-based process execution — the seam every real build/install/pkg-config path shells out through |
| `EnvironmentManager` | `CXPM::Utils::Unix` | `PATH` lookup (`which`) and environment-variable reads |

## Two manifest formats, one internal representation

`package.cpp` is a real C++ translation unit: it constructs `Target`/`Project` objects at static
(or, in the current implementation, dynamic-library-load) initialization time, then a generated
loader stub exposes an `extern "C" get_project()` symbol that `ProjectManager` resolves. This
means the manifest language is exactly as expressive as C++ itself — no custom parser, no schema
to keep in sync — at the cost of every `cxpm --build` compiling and loading a fresh shared object
before doing any real build work (see A21/A22 in [srs-architecture.md](srs-architecture.md) for
the concrete cost and a naming-collision caveat this currently has).

`package.json` is the declarative alternative: `ProjectManager::load_project` parses it straight
into the same `ProjectDescriptor` the `.cpp` path produces, with no compiler invocation and no
`dlopen` at all (see [srs-json-manifests.md](srs-json-manifests.md)). Everything downstream of
"the manifest is loaded" — toolchain selection, building, installing — is identical regardless of
which format was on disk, because both paths converge on the same struct.
`package.cpp` takes precedence when both exist in the same directory.

## Toolchains as plugins, not built-ins

A toolchain is not hardcoded into `cxpm`; it is described by a `ToolchainDescriptor`, sourced
either from a `toolchain.cpp` plugin (compiled on demand, using whatever C++ compiler
`EnvironmentManager::which("c++")` resolves on the machine actually running `cxpm`) or a
`toolchain.json` (parsed directly, no compilation). `modules/toolchains/{gcc,g++,nvcc}` ship as
`toolchain.cpp` source (`install(FILES src/toolchain.cpp DESTINATION share/cxpm/toolchains/<name>)`).
This plugin model is what lets `nvcc` (an entirely different toolchain family) sit alongside
`gcc`/`g++` without any special-casing in `cxpm` itself; the `.cpp`/`.json` choice is orthogonal
to that — see [srs-json-manifests.md](srs-json-manifests.md).

## Sandboxed descriptor extraction

Both manifest formats ultimately need a `ProjectDescriptor`/`ToolchainDescriptor` in the running
`cxpm` process's memory. For `.json`, that's just what the parser produces. For `.cpp`, that used
to mean `dlopen`-ing the compiled `.so` directly into `cxpm`'s own address space — so a project's
(or a dependency's) `package.cpp`/`toolchain.cpp` static initializers ran with the full privileges
of the `cxpm` process the instant `dlopen()` returned. `CXPM::Controllers::DescriptorSandbox` now
does that step in a `posix_spawn`'d, resource-limited child process instead, communicating the
result back as JSON — reusing the exact same codec `package.json` is parsed with. See
[srs-sandbox.md](srs-sandbox.md) for the full design and its explicitly accepted limitations.

## Honest status

This architecture is functional today — see the `cli_build_example_executable`/
`cli_build_example_executable_json` integration tests in
[srs-ctest-tooling.md](srs-ctest-tooling.md) for reproducible end-to-end builds through both
manifest formats and the sandboxed descriptor-loading path — but several parts are stubbed, dead,
or internally inconsistent. [srs-architecture.md](srs-architecture.md) itemizes every such flaw
found while writing this documentation, with file:line citations and a suggested correction for
each; several have since been fixed, including one — the `CXPM::Controllers`/`::Controllers`
namespace split (item A17) — that broke this codebase's own build while implementing
[srs-sandbox.md](srs-sandbox.md), turning a documented-but-latent flaw into concrete proof of why
it mattered.
