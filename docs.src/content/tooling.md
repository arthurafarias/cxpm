---
title: Tooling
---

# The `cxpm` command-line tool

`cxpm` (`applications/cxpm`) is a thin process front end: `applications/cxpm/src/cxpm.cpp`
constructs a `CXPM::Views::ApplicationView` and delegates everything to it. Argument parsing,
build orchestration, install logic and manifest generation all live in the header-only
`cxpm-interface` library. A second, small executable, `cxpm-descriptor-sandbox`, is described
separately under [Sandboxed descriptor extraction](#sandboxed-descriptor-extraction) below. See
[quickstart.md](quickstart.md) for a recorded, end-to-end terminal walkthrough of the commands
below.

`cxpm` is a git-style subcommand CLI — `cxpm <command> [<args>]` — implemented on top of a small,
reusable command-line framework in `cxpm-interface` itself
(`CXPM::Modules::ProgramOptions::{CommandRegistry,CommandLineParser,HelpFormatter}`). See
[srs-cli-subcommands.md](srs-cli-subcommands.md) for the full contract and the framework's design;
this page is the day-to-day reference.

```text
usage: cxpm <command> [<args>]

commands:
    build <directory>:                build the project whose package.cpp/package.json lives in <directory>
    install <directory> [--prefix <path>]: install the project (default prefix: /usr/local)
    generate <kind> [directory] [--force]: generate a starter manifest; kind is one of
                                        package-cpp, package-json, toolchain-cpp, toolchain-json
                                        (directory defaults to '.'; add --force to overwrite)
    help [<command>]:                 show this message, or detailed help for one command

-h/--help works on every command (`cxpm <command> --help`, or `cxpm help <command>`).
`uninstall` is not yet implemented — see srs-architecture.md item A13.
```

## Building a project

```bash
cxpm build .
```

`build` requires exactly one directory argument containing a `package.cpp` **or** `package.json`
(`Views::ApplicationView::assert_project_directory` validates this, and that the directory exists,
before doing anything else — see [srs-json-manifests.md](srs-json-manifests.md)). If both are
present, `package.cpp` takes precedence. `cxpm` loads the manifest into a `Project`, resolves a
toolchain for each target's `.language`, and shells out to that toolchain's compiler, linker and
archiver.

## Installing a project

```bash
cxpm install . --prefix /usr/local
```

`install` builds the project first, then copies already-built artifacts into a POSIX-aligned
prefix layout:

- Shared libraries → `<prefix>/lib`
- Static libraries → `<prefix>/lib`
- Executables → `<prefix>/bin`
- Headers → `<prefix>/include/<project-name>`
- pkg-config files → `<prefix>/lib/pkgconfig/<project-name>.pc`

`--prefix` defaults to `/usr/local` when omitted.

## Generating a starter manifest

```bash
cxpm generate package-json .        # or package-cpp, toolchain-cpp, toolchain-json
cxpm generate toolchain-json ./my-toolchain --force
```

`generate` writes a starter `package.cpp`/`package.json`/`toolchain.cpp`/`toolchain.json` into
`directory` (default `.`, created if missing), refusing to overwrite an existing file unless
`--force` is also given. See [srs-generate.md](srs-generate.md) (its content requirements are
unchanged by the move to a subcommand — only the invocation syntax moved).

## Describing a project: `package.cpp` or `package.json`

The `.cpp` and `.json` forms describe the exact same `ProjectDescriptor`; pick whichever fits —
see [srs-json-manifests.md](srs-json-manifests.md) for the full requirements, and
`cxpm generate package-cpp`/`cxpm generate package-json` to produce a starting point for
either.

```cpp
#include <CXPM/Target.hpp>
#include <CXPM/Project.hpp>

using namespace CXPM;

auto example = Target()
                   .name_set("example-executable")
                   .version_set("1.0.0")
                   .type_set("executable")
                   .sources_append({"src/main.cpp"})
                   .options_append({"-fPIE", "-fstack-protector-all"})
                   .link_libraries_append({"m"})
                   .include_directories_append({"src"})
                   .create();

auto project = Project().add(example).create();
```

Use `Target`/`Project` (not the bare `TargetDescriptor`/`ProjectDescriptor` structs, which have no
builder methods) — see [srs-architecture.md item A30](srs-architecture.md#41-fixed-in-this-change)
for a past README example that got this wrong.

```json
{
  "targets": [
    {
      "name": "example-executable",
      "version": "1.0.0",
      "type": "executable",
      "sources": ["src/main.cpp"],
      "options": ["-fPIE", "-fstack-protector-all"],
      "link_libraries": ["m"],
      "include_directories": ["src"]
    }
  ],
  "toolchains": []
}
```

A field omitted from the JSON falls back to `TargetDescriptor`'s own documented default (e.g. a
target with no `"type"` still defaults to `"executable"`) — see
[examples/executable-json](https://github.com/arthurafarias/cxpm/tree/master/examples/executable-json)
for a complete, buildable example.

## Describing a toolchain: `toolchain.cpp` or `toolchain.json`

```cpp
#include <CXPM/Toolchain.hpp>

CXPM::ToolchainDescriptor toolchain =
    CXPM::Toolchain()
        .name_set("g++")
        .version_set("generic")
        .compiler_executable_set("/usr/bin/g++")
        .linker_executable_set("/usr/bin/g++")
        .archiver_executable_set("/usr/bin/ar")
        .language_set("c++");
```

```json
{
  "name": "g++",
  "version": "generic",
  "language": "c++",
  "compiler_executable": "/usr/bin/g++",
  "linker_executable": "/usr/bin/g++",
  "archiver_executable": "/usr/bin/ar"
}
```

`cxpm` discovers `toolchain.cpp`/`toolchain.json` files on a search path seeded from the
`cxpm_BUILD_EXTRA_MODULES_PATH` environment variable plus a fixed list of install-prefix-relative
directories (see [architecture.md](architecture.md)). A `toolchain.cpp` is compiled on demand and
its descriptor extracted through the sandbox described below; a `toolchain.json` is parsed
directly, with no compilation step at all. `modules/toolchains/{gcc,g++,nvcc}` are the three
toolchains shipped with cxpm itself, installed as source under
`share/cxpm/toolchains/<name>/toolchain.cpp`.

## Sandboxed descriptor extraction

Loading a compiled `package.cpp`/`toolchain.cpp` requires running whatever static-initializer code
that `.cpp` file contains. `cxpm` does this in a separate, resource-limited child process
(`cxpm-descriptor-sandbox`) rather than inside the main `cxpm` process — see
[srs-sandbox.md](srs-sandbox.md) for the full design. This is transparent to normal use; it can be
disabled (falling back to the pre-sandbox direct-load behavior) with:

```bash
CXPM_SANDBOX_DISABLE=1 cxpm build .
```

`package.json`/`toolchain.json` never need this at all, since parsing them never executes code.

## Testing `cxpm` itself

The tooling described on this page is *what `cxpm` does for its users*. A separate CMake/CTest
facility exists for *testing* `cxpm` — registering `cxpm build`/`install`/`generate`
invocations (and direct `cxpm-descriptor-sandbox` calls) as CTest tests with fixtures, labels and
exit-code assertions. See [srs-ctest-tooling.md](srs-ctest-tooling.md) and
[testing.md](testing.md) for that.
