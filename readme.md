![cxpm](misc/images/logo-banner-path.svg)

cxpm is an experimental package manager for C++ that aims to simplify project configuration by using C++ itself as the declarative language. Instead of relying on separate configuration formats, cxpm allows projects and packages to describe themselves directly in standard C++ code.

The design is inspired by modern ecosystems such as **Julia**, **Swift**, and their language-native package declarations.

This project is at an early stage. It already builds, tests, packs and installs itself, with no other build system involved.

The website is at <https://01.dev.soft-forge.dev/cxpm/> and the package index at <https://01.dev.soft-forge.dev/cxpm/index/>.

If you're interested in discussing or contributing to this idea, feel free to fork the repository and open a proposal.

# Download Binnaries

# CX Libraries

 - [Pacman](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cx-0.1.0-1-x86_64.pkg.tar.zst)
 - [DEB](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cx_0.1.0_amd64.deb)
 - [RPM](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cx-0.1.0-1.x86_64.rpm)

# CX Interface

 - [Pacman](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-interface-0.2.0-1-x86_64.pkg.tar.zst)
 - [DEB](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-interface_0.2.0_amd64.deb)
 - [RPM](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-interface-0.2.0-1.x86_64.rpm)

# CXPM Application

 - [Pacman](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cx-pm-0.2.0-1-x86_64.pkg.tar.zst)
 - [DEB](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm_0.2.0_amd64.deb)
 - [RPM](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-0.2.0-1.x86_64.rpm)

# Toolchain Clang++

 - [Pacman](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cx-pm-toolchain-clang++-0.2.0-1-x86_64.pkg.tar.zst)
 - [DEB](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-clang++-0.2.0-1.x86_64.rpm)
 - [RPM](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-clang++-0.2.0-1.x86_64.rpm)

# Toolchain Clang

 - [Pacman](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cx-pm-toolchain-clang-0.2.0-1-x86_64.pkg.tar.zst)
 - [DEB](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-clang_0.2.0_amd64.deb)
 - [RPM](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-clang-0.2.0-1.x86_64.rpm)

# Toolchain GCC

 - [Pacman](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-gcc-0.2.0-1-x86_64.pkg.tar.zst)
 - [DEB](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-gcc_0.2.0_amd64.deb)
 - [RPM](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-gcc-0.2.0-1.x86_64.rpm)

# Toolchain G++

 - [Pacman](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-g++-0.2.0-1-x86_64.pkg.tar.zst)
 - [DEB](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-g++_0.2.0_amd64.deb)
 - [RPM](https://github.com/arthurafarias/cxpm/releases/download/v0.2.0/cxpm-toolchain-g++-0.2.0-1.x86_64.rpm)

# What a project looks like

```
- include/greeter/greeter.hpp
- src/greeter.cpp
- package.cpp
```

with a `package.cpp` such as

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

A project has one target. It sees only what it declares: `fmt` above is resolved to another
project of the same workspace, to a repository cxpm fetches, to a package a package index
publishes, or to a package of the sysroot, and its include paths and libraries come from there. A `package.json` with the same fields works
where no C++ is wanted.

The philosophy is a small, modern, expressive system: no macro layers, no special DSL. Your C++
project is described in C++, much like package.json, Package.swift or Pkg.jl.

## A manifest written for 0.7

0.7 spelled the API `CXPM::Target`, `CXPM::CompilerOptions::StandardCxx23` and
`#include <CXPM/Target.hpp>`. 0.8 kept forwarding headers under `include/CXPM` for one
release; 0.9 removes them. To move a manifest over:

- include `<cx/pm/project.hpp>` and `<cx/pm/target.hpp>`, and use `namespace cx::pm`;
- write the types and options in lower_snake_case: `target()`, `project()`,
  `compiler_options::standard_cxx23`;
- rename a global variable called `cx`. The headers declare `namespace cx`, so
  `auto cx = target()...` at namespace scope no longer compiles
  (`'auto cx' redeclared as different kind of entity`). `auto cx_libraries = ...` works.

## Once cxpm is installed

It is the build tool of this repository too:

```bash
cxpm build      # every project of the workspace
cxpm test       # the unit suite and the command line tests
cxpm install    # pack and install
cxpm uninstall
```

## A manifest that sets install scripts

Up to descriptor schema 5 a target named the scripts its package runs on installation and
removal. Schema 6 makes them tasks of the project:

| Before | Now |
|---|---|
| `target().post_install_script_set("packaging/postinst.in")` | `project().task_file_add("package_post_install", "packaging/postinst.in")` |
| `pre_remove_script_set(...)` | `task_file_add("package_pre_remove", ...)` |
| `post_remove_script_set(...)` | `task_file_add("package_post_remove", ...)` |
| `"post_install_script": "packaging/postinst.in"` in `target` | `"tasks": {"package_post_install": {"file": "packaging/postinst.in"}}` beside `target` |

The scripts are packed as before, with their `@NAME@`s replaced. A package.cpp that still calls
one of the old setters fails to compile with a message naming the task, and a package.json that
still sets one fails to load the same way.

# Using it

```bash
cxpm project create greeter --type static-library --tests --example
cd greeter
cxpm build              # fetch, configure and build what is stale
cxpm test               # build and run the tests projects
cxpm pack               # .cxpm/packages/greeter-0.1.0.cxpm.tar
cxpm install --prefix /usr/local
cxpm uninstall --prefix /usr/local
```

| Command | What it does |
|---|---|
| `fetch` | Clones the dependencies that name a repository or that the package index publishes, and locks their commits |
| `configure` | Chooses a toolchain and resolves every dependency, once |
| `build` | Compiles what changed, in parallel |
| `test` | Builds and runs the workspace's tests projects |
| `pack` | Stages the target and makes a package: a plain archive, or a pacman, rpm or deb package |
| `run` | Runs a task the project declares, `cxpm run lint -- --fix`; alone, lists them |
| `install`, `uninstall` | Hands the package to the package manager; given a name, installs the prebuilt package the index publishes |
| `generate`, `project create` | Writes manifests and whole project skeletons |
| `search`, `view`, `login`, `whoami`, `publish` | Looks packages up in a package index and publishes a version of one |
| `deprecate`, `yank`, `tag`, `owner`, `access` | Changes a package published already: what to use instead, versions taken back, named versions, who publishes it |
| `dependency add`, `remove`, `list`, `sync`, `update` | Edits the dependencies in the manifest and fetches them |
| `outdated` | Lists the dependencies of the index that a newer version exists for, or whose locked version is yanked or deprecated |
| `repl` | A session over these commands; `cxpm` alone opens it at a terminal |
| `serve` | Runs a package index; cxpm-web-index, a separate application, is the same index with accounts and a page |

Manifests are compiled and loaded in a separate, resource-limited process, and that process and
the build itself run in a namespace with no network and no view of your home directory when
bubblewrap is available.

`build` and the commands that build compile one source per core at a time. `--jobs N` (`-j N`)
sets another number, and `CXPM_BUILD_JOBS` does the same when the option is not given; one job
builds exactly what the parallel build does, one source after another. The manifest process is
started through `cxpm-descriptor-sandbox`, found through `CXPM_SANDBOX_HELPER`, then beside
`cxpm` or on `PATH`, then in the build tree cxpm was compiled in, then under the install
prefix; when none has it, the error names every place it looked.

`publish` tells the index which commit of the repository a version is, and sends with it the
deb, rpm and pacman packages `cxpm pack --package-manager deb|rpm|pacman` left in
`.cxpm/packages` for that version; `publish --pack` makes the three first. A version with
packages can be published from a checkout that is not a pushed commit, or from no repository
at all: the index then serves only the packages.

`cxpm serve` takes each package before its version: it refuses a file whose name is not one
the three backends give that version, and a deb or rpm whose control file or header names
another package, version or machine (a pacman package is checked only for its zstd magic
number), and files of a version already published. Once the version names a file, it is served
at `<index>/<name>/<version>/-/<file>` with its SHA-256 in the `x-sha256` field. A package may
be up to 256 MiB. `fetch` builds from source, so it passes over a version that has no
repository; install that one's packages with the system's package manager.

`publish --private` makes a package its owner's alone: read with any other login, or with
none, the index has no such package, so it is not listed, its versions and packages are not
served, and `fetch` does not find it. `cxpm` sends the stored token with every read, so the
owner fetches it like any other. `publish --public` makes it public again; a publication with
neither keeps what the package is.

## Tasks and hooks

A project declares tasks, a name and the shell text it runs, beside its target:

```cpp
auto project = cx::pm::project()
                   .target_set(greeter)
                   .task_add("lint", "clang-tidy src/*.cpp -- -Iinclude")
                   .task_add("pre_build", "./tools/generate-version.sh > include/version.hpp")
                   .task_file_add("package_post_install", "packaging/postinst.in")
                   .create();
```

`cxpm run lint -- --fix` runs one with `/bin/sh -e -c` in the project directory, `--fix` as its
`$1`, and exits with its status; `cxpm run` lists them. `pre_<stage>` and `post_<stage>` run
around `fetch`, `configure`, `build`, `test`, `pack`, `install` and `publish`, for the project
and its members but never for a dependency; `--no-hooks` or `CXPM_NO_HOOKS=1` skips them.
`package_post_install`, `package_pre_remove` and `package_post_remove` are not run here: `pack`
puts them into the deb, rpm or pacman package. The
[command page](https://01.dev.soft-forge.dev/cxpm/tooling/#tasks-and-hooks) has the whole
contract.

## Keeping a package and its dependencies

`cxpm view fmt` shows a package: its owner and maintainers, its tags, its versions newest
first and the yanked ones apart; `cxpm view fmt@10.2.1` shows one version, with its commit,
dependencies and packages, and `--json` prints what the index answered. `cxpm outdated` lists
the project's dependencies of the index whose locked version is not the newest, with the newest
the manifest accepts and the newest published, and says when the locked one is yanked or
deprecated. `cxpm dependency update fmt` moves one dependency to the newest version the
manifest accepts (`cxpm dependency update` moves every one) and pins it; when that fails, the
lock stays as it was.

Whoever publishes a package changes it afterwards, with the token of `cxpm login`:

```bash
cxpm deprecate fmt "use fmt2"            # or fmt@10.0.0, one version; --undo takes it back
cxpm yank fmt@10.2.0                     # --undo puts it back
cxpm tag add fmt@11.0.0-rc1 next         # tag remove fmt next, tag list fmt
cxpm owner add ada fmt                   # ada publishes fmt too; owner remove ada fmt
cxpm owner offer ada fmt                 # ada runs `cxpm owner take fmt`, or owner decline fmt
cxpm access fmt private                  # or public
```

A yanked version is still read and downloaded, and a project whose lock pins it keeps it, with a
warning, until `cxpm dependency update` moves it; nothing new resolves to it, a tag cannot point
at it, and its number is never published again. `fetch` warns about a deprecated package or
version, with the message saying what to use. Maintainers publish, deprecate, yank and tag; the
owner alone adds or removes them, makes the package private, and offers it to another account,
which becomes its owner when it takes it. `cxpm serve` and cxpm-web-index answer these
changes at `/-/packages/<name>/...`, so either index keeps packages the same way.

## Installing a published package

A version published with `cxpm publish --pack` carries a deb, an rpm and a pacman package.
`cxpm install hola` installs the newest version of hola from the one this system takes, without
building anything:

```bash
cxpm install hola                         # the newest version
cxpm install hola@1.0.0                   # or hola@">= 1.0"
cxpm install hola --root /srv/sysroot --package-manager pacman
cxpm uninstall hola
```

The package manager is `--package-manager`, else the one of the root's distribution, read from
its os-release. The file is the one built for this machine (`uname -m`), and it is installed only
once its SHA-256 is the one the index lists. A version with no such file is not installed:
cxpm says which files it has, and that the way left is building it, as a dependency of a project
or from its repository, with `cxpm install` there. `install` and `uninstall` take a name only
where no directory of that name is; `./hola` is always the directory.

## Scoped names

A package of the index may be scoped, as `@acme/widget`. The account `acme` owns the scope: it
publishes the first version of a name in it and adds whoever else publishes it (`cxpm owner add
ada @acme/widget`). A project publishes under a scope by declaring it beside its name, and every
command that takes a package name takes the scoped one:

```json
{"schema": 3, "target": {"name": "widget", "scope": "acme", "version": "1.2.0", "type": "static-library"}}
```

```bash
cxpm dependency add @acme/widget          # writes "@acme/widget >= 1.2.0"
cxpm fetch
cxpm install @acme/widget@1.2.0
```

The scope is how the index tells packages apart. On a machine, the package is `widget`: its
pkg-config module, the name the build and the lock know it by, `.cxpm/sources/widget`, its
deb, rpm and pacman packages (`widget_1.2.0_amd64.deb`), and the package the system's package
manager installs. So two packages that differ only by scope can't be in one build, or on one
system, at once.

## Signing in and plans

The public index signs people in through Soft Forge Payments, which also sells its plans:

| Plan | Price | What it publishes |
|---|---|---|
| Free | nothing | one public package, with as many versions as wanted |
| Indexing | monthly | any number of packages, public or private |

Reading, searching and fetching public packages need no account. `cxpm login` with no token
starts a sign-in: it prints an address and a code, the person signs in on the platform there
(taking the Free plan the first time) and confirms the code, and `cxpm` stores the token the
index gives it. The index, not `cxpm`, talks to the platform: it starts the sign-in with
`/v1/sso/start`, asks `/v1/sso/wait` while `cxpm login` waits on `/-/login/wait`, and keeps
the platform's grant to itself. `cxpm` only ever holds a token of the index, and the index
sees only the SHA-256 of the secret `cxpm login` makes, so nobody else can collect the token.

The plan is checked when publishing. A second package on Free, or `--private` without
Indexing, is refused with 402 and the address of the platform's page where the plan is taken.
When a plan ends, nothing published is taken away: packages stay as they are, and only new
packages over the Free plan's one are refused. `cxpm serve` has no plans: its operator makes
the accounts (`--add-account`), and each logs in with `cxpm login --token`. `cxpm logout`
revokes that token there, and the operator makes another with `--add-account` again.

`cxpm help` lists the commands and `cxpm help <command>` describes one. A mistyped command
line, such as an unknown command or option or a missing argument, fails with one line naming
the command and pointing at its help.

The documentation is under [docs.src/content](docs.src/content): start with
[quickstart.md](docs.src/content/quickstart.md), and [tooling.md](docs.src/content/tooling.md)
is the reference.

# Roadmap

cxpm is being redesigned to replace CMake rather than sit beside it: fetch, configure, build,
pack and install against a sysroot made of a project's declared dependencies. The milestones
and what is still open are described in [roadmap.md](docs.src/content/roadmap.md) and tracked issue by issue in
`planning.pb`.

Arthur Farias <afarias.arthur@gmail.com>
