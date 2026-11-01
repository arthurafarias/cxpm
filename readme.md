![cxpm](misc/images/logo-banner-path.svg)

cxpm is an experimental package manager for C++ that uses C++ itself as the declarative
language: a project describes itself in a `package.cpp`, the way a Swift package does in
`Package.swift` or a Julia one in `Project.toml`. cxpm fetches, configures, builds, tests,
packs and installs projects against a sysroot made of their declared dependencies, with no
other build system involved; it builds, tests and packs itself the same way.

- Handbook, with the manifest format, the lifecycle, tasks and hooks and the package index:
  <https://01.dev.soft-forge.dev/cxpm/>
- Command line reference, the text of `cxpm help` for every command:
  <https://arthurafarias.github.io/cxpm/>
- Public package index: <https://01.dev.soft-forge.dev/cxpm/index/>

This repository publishes the prebuilt packages. The source is not public: cxpm is built on
the cx libraries, which are closed source.

## Install

Release **0.3.0** is built for x86_64, as a package for each package manager. Every
distribution installs the same six packages:

| Package | What it is |
|---|---|
| `cx` | The cx libraries cxpm is built with (headers and pkg-config files) |
| `cxpm-interface` | The headers a `package.cpp` includes: `<cx/pm/project.hpp>`, `<cx/pm/target.hpp>` |
| `cxpm-descriptor-sandbox` | The helper that compiles and loads a manifest in a confined process |
| `cxpm-toolchain-gcc`, `-g++`, `-clang`, `-clang++`, `-nvcc` | The toolchain modules; install the ones for the compilers you have, `g++` at least |
| `cxpm` | The command |

Install all of them at once: the package manager orders them. You need a C++23 compiler
(`g++` 14 or `clang++` 18 or newer), `git` and `pkg-config` on the machine; cxpm finds them
on `PATH`.

### Arch Linux, Manjaro (pacman)

```sh
curl -fsSLO https://github.com/arthurafarias/cxpm/releases/download/v0.3.0/cxpm-0.3.0-arch-x86_64.tar
tar -xf cxpm-0.3.0-arch-x86_64.tar
sudo pacman -U --needed cxpm-0.3.0-arch-x86_64/*.pkg.tar.zst
```

### Debian 12 and 13, Ubuntu 24.04 and later (deb)

```sh
curl -fsSLO https://github.com/arthurafarias/cxpm/releases/download/v0.3.0/cxpm-0.3.0-debian-amd64.tar
tar -xf cxpm-0.3.0-debian-amd64.tar
sudo apt install ./cxpm-0.3.0-debian-amd64/*.deb
```

### Fedora, RHEL 9 and 10, Amazon Linux 2023 (rpm)

```sh
curl -fsSLO https://github.com/arthurafarias/cxpm/releases/download/v0.3.0/cxpm-0.3.0-rpm-x86_64.tar
tar -xf cxpm-0.3.0-rpm-x86_64.tar
sudo dnf install ./cxpm-0.3.0-rpm-x86_64/*.rpm
```

Each bundle holds the packages of one format; the single packages are attached to the
[release](https://github.com/arthurafarias/cxpm/releases/tag/v0.3.0) too, for installing
one at a time (`sudo pacman -U cxpm-0.3.0-1-x86_64.pkg.tar.zst`, `sudo apt install
./cxpm_0.3.0_amd64.deb`, `sudo dnf install ./cxpm-0.3.0-1.x86_64.rpm`).

Then:

```sh
cxpm help                 # the commands; cxpm help <command> describes one
cxpm project create greeter --type static-library --tests --example
cd greeter && cxpm build && cxpm test
```

### Remove

```sh
sudo pacman -Rs cxpm cxpm-toolchain-gcc cxpm-toolchain-g++ cxpm-toolchain-clang cxpm-toolchain-clang++ cxpm-toolchain-nvcc cxpm-descriptor-sandbox cxpm-interface cx
sudo apt remove cxpm cxpm-toolchain-gcc cxpm-toolchain-g++ cxpm-toolchain-clang cxpm-toolchain-clang++ cxpm-toolchain-nvcc cxpm-descriptor-sandbox cxpm-interface cx
sudo dnf remove cxpm cxpm-toolchain-gcc cxpm-toolchain-g++ cxpm-toolchain-clang cxpm-toolchain-clang++ cxpm-toolchain-nvcc cxpm-descriptor-sandbox cxpm-interface cx
```

## What a project looks like

```
- include/greeter/greeter.hpp
- src/greeter.cpp
- package.cpp
```

```cpp
#include <cx/pm/project.hpp>
#include <cx/pm/target.hpp>

using namespace cx::pm;

auto project = cx::pm::project()
                   .target_set(cx::pm::target()
                                   .name_set("greeter")
                                   .version_set("0.1.0")
                                   .type_set("static-library")
                                   .sources_append({"src/*.cpp"})
                                   .include_directories_append({"include"})
                                   .options_append({compiler_options::standard_cxx23})
                                   .dependencies_append({"fmt"})
                                   .create())
                   .create();
```

A project has one target and sees only what it declares: `fmt` above is resolved to another
project of the same workspace, to a repository cxpm fetches, to a package the package index
publishes, or to a package of the sysroot. A `package.json` with the same fields works where no
C++ is wanted.

```sh
cxpm build              # fetch, configure and build what is stale
cxpm test               # build and run the tests projects
cxpm pack               # a plain archive, or a pacman, rpm or deb package
cxpm install --prefix /usr/local
cxpm publish            # a version on the package index, with its packages
```

| Command | What it does |
|---|---|
| `fetch`, `configure`, `build`, `test`, `pack`, `install`, `uninstall` | The lifecycle of a project, against a sysroot of its dependencies |
| `run` | Runs a task the project declares; `pre_<stage>` and `post_<stage>` tasks run as hooks |
| `generate`, `project create` | Writes manifests and whole project skeletons |
| `search`, `view`, `login`, `whoami`, `publish` | Looks packages up in a package index and publishes a version of one |
| `deprecate`, `yank`, `unpublish`, `tag`, `owner`, `access` | Changes a package published already |
| `dependency add`, `remove`, `list`, `sync`, `update`, `outdated` | Edits the dependencies in the manifest and keeps them current |
| `repl` | A session over these commands; `cxpm` alone opens it at a terminal |
| `serve` | Runs a package index of your own |

The full text of every command's help is on the
[command line reference](https://arthurafarias.github.io/cxpm/), and the handbook at
<https://01.dev.soft-forge.dev/cxpm/> explains the manifest, the lifecycle, tasks and hooks,
toolchains and the package index.

## What is new in 0.3.0

- `cxpm unpublish <name> [<version>]` deletes a version, or a package, from the index. It says
  what goes and asks for the name to be typed (`--yes` for scripts); the index refuses the last
  version and anything another owner's package depends on, pointing at `cxpm yank`.
- `cxpm fetch` fails, and says the way out, when the lock pins a version the index no longer
  has; `cxpm outdated` lists such a dependency as gone.
- `cxpm serve` answers `DELETE /<name>` and `DELETE /<name>/<version>` with the index's
  contract.
- Tasks and lifecycle hooks: `cxpm run`, `pre_<stage>`/`post_<stage>`, and the package hooks
  `package_post_install`, `package_pre_remove`, `package_post_remove`.

## Contributing

If you're interested in discussing this idea, open an issue here. The source is not open, so
there is nothing to fork; proposals and bug reports are welcome.

Arthur Farias
