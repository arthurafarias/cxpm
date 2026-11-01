---
title: cxpm command line reference
---

# cxpm 0.3.0: command line reference

This page is the text `cxpm help` and `cxpm help <command>` print, for every command of cxpm 0.3.0. The same text is at hand on any machine with cxpm installed. The [readme](https://github.com/arthurafarias/cxpm#install) says how to install cxpm on each distribution; the handbook, with the manifest format, the lifecycle, tasks and hooks and the package index, is at <https://01.dev.soft-forge.dev/cxpm/>.

```text
Usage: cxpm [OPTIONS] COMMAND [ARGS]...

A package manager for C++ using C++

Commands:
  fetch         Download the dependencies a project fetches, resolve the ones the package index
                publishes, and pin them all in .cxpm/lock.json
  configure     Resolve the toolchain and dependencies again; build and install the dependencies
                built from source
  build         Build a project in a directory containing package.cpp/package.json
  test          Build a project, then build and run its tests projects
  run           Run a task the project declares, with the arguments after `--`; without a name, list
                the tasks
  pack          Build a project and pack it for the toolchain's package manager
  install       Pack a project, then install it with the toolchain's package manager; or install a
                package of the index from its prebuilt package
  uninstall     Remove what `cxpm install` installed
  search        List the packages of the index, or the ones matching a text
  login         Log in to an index and store the token `cxpm publish` presents to it
  logout        Forget the token stored for an index
  publish       Publish the project's version to the index, as a commit of its repository, with the
                packages `cxpm pack` made of it
  whoami        Show the account logged in to an index
  view          Show a package of the index: its versions, tags, owner and maintainers; or one
                version, with its commit, dependencies and files
  outdated      List the dependencies of the index that a newer version exists for, or whose locked
                version is yanked or deprecated
  deprecate     Tell those who fetch a package, or one version of it, what to use instead
  yank          Take a version back: nothing new resolves to it, and the projects that lock it keep
                it
  unpublish     Delete a version, or the whole package, from the index: the number is never
                published again, and a project locked to it fails its next fetch. Asks for the name
                to be typed; yank takes a version back without breaking anybody
  access        Make a package private, seen and fetched only by its owner and maintainers, or
                public
  repl          Open a session over these commands, with history and Tab completion; `cxpm` alone at
                a terminal does the same
  serve         Serve a package index over HTTP
  generate      Scaffold a starter package/toolchain manifest
  help          Show this message, or detailed help for one command
  tag           Name versions of a package: `cxpm tag add fmt@11.0.0-rc1 next`
  owner         Who owns a package and who else publishes it
  dependency    Add, remove and list the project's dependencies
  project       Create projects

Options:
  -h, --help  Show this message and exit.
```

## Commands

- [cxpm fetch](#cxpm-fetch)
- [cxpm configure](#cxpm-configure)
- [cxpm build](#cxpm-build)
- [cxpm test](#cxpm-test)
- [cxpm run](#cxpm-run)
- [cxpm pack](#cxpm-pack)
- [cxpm install](#cxpm-install)
- [cxpm uninstall](#cxpm-uninstall)
- [cxpm search](#cxpm-search)
- [cxpm login](#cxpm-login)
- [cxpm logout](#cxpm-logout)
- [cxpm publish](#cxpm-publish)
- [cxpm whoami](#cxpm-whoami)
- [cxpm view](#cxpm-view)
- [cxpm outdated](#cxpm-outdated)
- [cxpm deprecate](#cxpm-deprecate)
- [cxpm yank](#cxpm-yank)
- [cxpm unpublish](#cxpm-unpublish)
- [cxpm access](#cxpm-access)
- [cxpm repl](#cxpm-repl)
- [cxpm serve](#cxpm-serve)
- [cxpm generate](#cxpm-generate)
- [cxpm tag](#cxpm-tag)
  - [cxpm tag add](#cxpm-tag-add)
  - [cxpm tag remove](#cxpm-tag-remove)
  - [cxpm tag list](#cxpm-tag-list)
- [cxpm owner](#cxpm-owner)
  - [cxpm owner list](#cxpm-owner-list)
  - [cxpm owner add](#cxpm-owner-add)
  - [cxpm owner remove](#cxpm-owner-remove)
  - [cxpm owner offer](#cxpm-owner-offer)
  - [cxpm owner take](#cxpm-owner-take)
  - [cxpm owner decline](#cxpm-owner-decline)
- [cxpm dependency](#cxpm-dependency)
  - [cxpm dependency add](#cxpm-dependency-add)
  - [cxpm dependency remove](#cxpm-dependency-remove)
  - [cxpm dependency list](#cxpm-dependency-list)
  - [cxpm dependency update](#cxpm-dependency-update)
  - [cxpm dependency sync](#cxpm-dependency-sync)
- [cxpm project](#cxpm-project)
  - [cxpm project create](#cxpm-project-create)

## cxpm fetch

```text
cxpm fetch - Download the dependencies a project fetches, resolve the ones the package index publishes, and pin them all in .cxpm/lock.json

Usage: cxpm fetch [OPTIONS] [directory]

Download the dependencies a project fetches, resolve the ones the package index publishes, and pin
them all in .cxpm/lock.json

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help            Show this message and exit.
  -t, --toolchain TEXT  Build with this toolchain instead of the one the target selects
  --sysroot TEXT        Build against this sysroot instead of the toolchain's
  --no-hooks            Do not run the pre_<stage> and post_<stage> tasks (also CXPM_NO_HOOKS=1)
  --index TEXT          The package index to use instead of CXPM_INDEX or the public one; `off` for
                        none
```

## cxpm configure

```text
cxpm configure - Resolve the toolchain and dependencies again; build and install the dependencies built from source

Usage: cxpm configure [OPTIONS] [directory]

Resolve the toolchain and dependencies again; build and install the dependencies built from source

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help            Show this message and exit.
  -t, --toolchain TEXT  Build with this toolchain instead of the one the target selects
  --sysroot TEXT        Build against this sysroot instead of the toolchain's
  --no-hooks            Do not run the pre_<stage> and post_<stage> tasks (also CXPM_NO_HOOKS=1)
  -j, --jobs INTEGER    How many sources compile at once (default: CXPM_BUILD_JOBS, or one per core)
                        [default: 0]
  -f, --force           Rebuild what is up to date
  -r, --reconfigure     Resolve the manifest, toolchain and dependencies again
```

## cxpm build

```text
cxpm build - Build a project in a directory containing package.cpp/package.json

Usage: cxpm build [OPTIONS] [directory]

Build a project in a directory containing package.cpp/package.json

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help            Show this message and exit.
  -t, --toolchain TEXT  Build with this toolchain instead of the one the target selects
  --sysroot TEXT        Build against this sysroot instead of the toolchain's
  --no-hooks            Do not run the pre_<stage> and post_<stage> tasks (also CXPM_NO_HOOKS=1)
  -j, --jobs INTEGER    How many sources compile at once (default: CXPM_BUILD_JOBS, or one per core)
                        [default: 0]
  -f, --force           Rebuild what is up to date
  -r, --reconfigure     Resolve the manifest, toolchain and dependencies again
```

## cxpm test

```text
cxpm test - Build a project, then build and run its tests projects

Usage: cxpm test [OPTIONS] [directory]

Build a project, then build and run its tests projects

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help            Show this message and exit.
  -t, --toolchain TEXT  Build with this toolchain instead of the one the target selects
  --sysroot TEXT        Build against this sysroot instead of the toolchain's
  --no-hooks            Do not run the pre_<stage> and post_<stage> tasks (also CXPM_NO_HOOKS=1)
  -j, --jobs INTEGER    How many sources compile at once (default: CXPM_BUILD_JOBS, or one per core)
                        [default: 0]
  -f, --force           Rebuild what is up to date
  -r, --reconfigure     Resolve the manifest, toolchain and dependencies again
```

## cxpm run

```text
cxpm run - Run a task the project declares, with the arguments after `--`; without a name, list the tasks

Usage: cxpm run [OPTIONS] [task] [arguments]...

Run a task the project declares, with the arguments after `--`; without a name, list the tasks

Arguments:
  task    The task to run (default: list them). pre_<stage> and post_<stage> also run around fetch,
          configure, build, test, pack, install and publish; package_post_install,
          package_pre_remove and package_post_remove go into the package
  arguments    Passed to the task as $1, $2...

Options:
  -h, --help            Show this message and exit.
  -C, --directory TEXT  The project directory (default: .) [default: .]
  -p, --prefix TEXT     The prefix a package_ hook's @NAME@s are expanded for (default: the
                        target's, or /usr/local)
```

## cxpm pack

```text
cxpm pack - Build a project and pack it for the toolchain's package manager

Usage: cxpm pack [OPTIONS] [directory]

Build a project and pack it for the toolchain's package manager

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help              Show this message and exit.
  -t, --toolchain TEXT    Build with this toolchain instead of the one the target selects
  --sysroot TEXT          Build against this sysroot instead of the toolchain's
  --no-hooks              Do not run the pre_<stage> and post_<stage> tasks (also CXPM_NO_HOOKS=1)
  -j, --jobs INTEGER      How many sources compile at once (default: CXPM_BUILD_JOBS, or one per
                          core) [default: 0]
  -f, --force             Rebuild what is up to date
  -r, --reconfigure       Resolve the manifest, toolchain and dependencies again
  -p, --prefix TEXT       Install prefix (default: the target's, the toolchain's, or /usr/local)
  --package-manager TEXT  Pack and install with this package manager instead of the toolchain's:
                          archive, pacman, rpm or deb
```

## cxpm install

```text
cxpm install - Pack a project, then install it with the toolchain's package manager; or install a package of the index from its prebuilt package

Usage: cxpm install [OPTIONS] [directory]

Pack a project, then install it with the toolchain's package manager; or install a package of the
index from its prebuilt package

Arguments:
  directory    The project directory (default: .), or a package of the index as name or
               name@constraint [default: .]

Options:
  -h, --help              Show this message and exit.
  -t, --toolchain TEXT    Build with this toolchain instead of the one the target selects
  --sysroot TEXT          Build against this sysroot instead of the toolchain's
  --no-hooks              Do not run the pre_<stage> and post_<stage> tasks (also CXPM_NO_HOOKS=1)
  -j, --jobs INTEGER      How many sources compile at once (default: CXPM_BUILD_JOBS, or one per
                          core) [default: 0]
  -f, --force             Rebuild what is up to date
  -r, --reconfigure       Resolve the manifest, toolchain and dependencies again
  -p, --prefix TEXT       Install prefix (default: the target's, the toolchain's, or /usr/local)
  --root TEXT             Install into this root instead of the toolchain's sysroot
  --package-manager TEXT  Pack and install with this package manager instead of the toolchain's:
                          archive, pacman, rpm or deb
  --index TEXT            The package index to use instead of CXPM_INDEX or the public one; `off`
                          for none
```

## cxpm uninstall

```text
cxpm uninstall - Remove what `cxpm install` installed

Usage: cxpm uninstall [OPTIONS] [directory]

Remove what `cxpm install` installed

Arguments:
  directory    The project directory (default: .), or the name of a package installed from the index
               [default: .]

Options:
  -h, --help              Show this message and exit.
  -t, --toolchain TEXT    Build with this toolchain instead of the one the target selects
  --sysroot TEXT          Build against this sysroot instead of the toolchain's
  -p, --prefix TEXT       Install prefix (default: the target's, the toolchain's, or /usr/local)
  --root TEXT             Install into this root instead of the toolchain's sysroot
  --package-manager TEXT  Pack and install with this package manager instead of the toolchain's:
                          archive, pacman, rpm or deb
```

## cxpm search

```text
cxpm search - List the packages of the index, or the ones matching a text

Usage: cxpm search [OPTIONS] [text]

List the packages of the index, or the ones matching a text

Arguments:
  text    Part of a name or description

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm login

```text
cxpm login - Log in to an index and store the token `cxpm publish` presents to it

Usage: cxpm login [OPTIONS]

Log in to an index and store the token `cxpm publish` presents to it

Options:
  -h, --help    Show this message and exit.
  --token TEXT  A token of the account, instead of confirming the sign-in on the payments platform
                (default: CXPM_TOKEN)
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm logout

```text
cxpm logout - Forget the token stored for an index

Usage: cxpm logout [OPTIONS]

Forget the token stored for an index

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm publish

```text
cxpm publish - Publish the project's version to the index, as a commit of its repository, with the packages `cxpm pack` made of it

Usage: cxpm publish [OPTIONS] [directory]

Publish the project's version to the index, as a commit of its repository, with the packages `cxpm
pack` made of it

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help            Show this message and exit.
  --index TEXT          The package index to use instead of CXPM_INDEX or the public one; `off` for
                        none
  --repository TEXT     The address others clone the project from (default: the origin remote)
  --revision TEXT       The branch, tag or commit to publish (default: HEAD)
  --package TEXT        The project to publish, by the name it builds, when the directory is a
                        workspace
  --pack                Pack the project for deb, rpm and pacman first, and send the three packages
  --private             Make the package private: only the account logged in sees and fetches it
  --public              Make a private package public again
  -t, --toolchain TEXT  With --pack, build with this toolchain instead of the one the target selects
  -j, --jobs INTEGER    With --pack, how many sources compile at once (default: CXPM_BUILD_JOBS, or
                        one per core) [default: 0]
  --no-hooks            Do not run the pre_<stage> and post_<stage> tasks (also CXPM_NO_HOOKS=1)
```

## cxpm whoami

```text
cxpm whoami - Show the account logged in to an index

Usage: cxpm whoami [OPTIONS]

Show the account logged in to an index

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm view

```text
cxpm view - Show a package of the index: its versions, tags, owner and maintainers; or one version, with its commit, dependencies and files

Usage: cxpm view [OPTIONS] package

Show a package of the index: its versions, tags, owner and maintainers; or one version, with its
commit, dependencies and files

Arguments:
  package    name, or name@version [required]

Options:
  -h, --help    Show this message and exit.
  --json        Print the index's record as it is
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm outdated

```text
cxpm outdated - List the dependencies of the index that a newer version exists for, or whose locked version is yanked or deprecated

Usage: cxpm outdated [OPTIONS] [directory]

List the dependencies of the index that a newer version exists for, or whose locked version is
yanked or deprecated

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm deprecate

```text
cxpm deprecate - Tell those who fetch a package, or one version of it, what to use instead

Usage: cxpm deprecate [OPTIONS] package [message]

Tell those who fetch a package, or one version of it, what to use instead

Arguments:
  package    name, or name@version [required]
  message    What to use instead

Options:
  -h, --help    Show this message and exit.
  --undo        Take the deprecation back
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm yank

```text
cxpm yank - Take a version back: nothing new resolves to it, and the projects that lock it keep it

Usage: cxpm yank [OPTIONS] package

Take a version back: nothing new resolves to it, and the projects that lock it keep it

Arguments:
  package    name@version [required]

Options:
  -h, --help    Show this message and exit.
  --undo        Put the version back
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm unpublish

```text
cxpm unpublish - Delete a version, or the whole package, from the index: the number is never published again, and a project locked to it fails its next fetch. Asks for the name to be typed; yank takes a version back without breaking anybody

Usage: cxpm unpublish [OPTIONS] package [version]

Delete a version, or the whole package, from the index: the number is never published again, and a
project locked to it fails its next fetch. Asks for the name to be typed; yank takes a version back
without breaking anybody

Arguments:
  package    name, or name@version [required]
  version    The version to delete; none deletes the package

Options:
  -h, --help    Show this message and exit.
  --yes         Do not ask for the name to be typed
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm access

```text
cxpm access - Make a package private, seen and fetched only by its owner and maintainers, or public

Usage: cxpm access [OPTIONS] package access

Make a package private, seen and fetched only by its owner and maintainers, or public

Arguments:
  package    The package's name [required]
  access    private or public [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm repl

```text
cxpm repl - Open a session over these commands, with history and Tab completion; `cxpm` alone at a terminal does the same

Usage: cxpm repl [OPTIONS]

Open a session over these commands, with history and Tab completion; `cxpm` alone at a terminal does
the same

Options:
  -h, --help  Show this message and exit.
```

## cxpm serve

```text
cxpm serve - Serve a package index over HTTP

Usage: cxpm serve [OPTIONS]

Serve a package index over HTTP

Options:
  -h, --help            Show this message and exit.
  --listen TEXT         Where to listen: unix:///path/to/socket, http://host:port or host:port
                        [default: http://127.0.0.1:7878]
  --host TEXT           The public address a proxy serves the index at, as in
                        https://example.org/cxpm; its path is accepted as a prefix of every request
  --database-path TEXT  The directory the index keeps its packages and accounts in [default:
                        /var/lib/cxpm/index]
  --add-account TEXT    Create this account (or renew its token), print the token and exit
```

## cxpm generate

```text
cxpm generate - Scaffold a starter package/toolchain manifest

Usage: cxpm generate [OPTIONS] kind [directory]

Scaffold a starter package/toolchain manifest

Arguments:
  kind    The manifest to write [choices: package-cpp, package-json, toolchain-cpp, toolchain-json]
          [required]
  directory    Where the manifest is written [default: .]

Options:
  -h, --help   Show this message and exit.
  -f, --force  Overwrite the target file if it already exists
```

## cxpm tag

```text
Usage: cxpm tag [OPTIONS] COMMAND [ARGS]...

Name versions of a package: `cxpm tag add fmt@11.0.0-rc1 next`

Commands:
  add       Point a tag at a version, wherever it pointed
  remove    Remove a tag
  list      List the tags of a package, and its latest version

Options:
  -h, --help  Show this message and exit.
```

## cxpm tag add

```text
Usage: cxpm tag add [OPTIONS] package tag

Point a tag at a version, wherever it pointed

Arguments:
  package    name@version [required]
  tag    The tag [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm tag remove

```text
Usage: cxpm tag remove [OPTIONS] package tag

Remove a tag

Arguments:
  package    The package's name [required]
  tag    The tag [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm tag list

```text
Usage: cxpm tag list [OPTIONS] package

List the tags of a package, and its latest version

Arguments:
  package    The package's name [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm owner

```text
Usage: cxpm owner [OPTIONS] COMMAND [ARGS]...

Who owns a package and who else publishes it

Commands:
  list       List the owner and the maintainers of a package
  add        Let an account publish a package, as a maintainer
  remove     Stop an account publishing a package
  offer      Offer a package to another account, which takes it with `cxpm owner take`
  take       Take a package offered to the account logged in
  decline    Withdraw an offer of a package, or decline one

Options:
  -h, --help  Show this message and exit.
```

## cxpm owner list

```text
Usage: cxpm owner list [OPTIONS] package

List the owner and the maintainers of a package

Arguments:
  package    The package's name [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm owner add

```text
Usage: cxpm owner add [OPTIONS] account package

Let an account publish a package, as a maintainer

Arguments:
  account    The account [required]
  package    The package's name [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm owner remove

```text
Usage: cxpm owner remove [OPTIONS] account package

Stop an account publishing a package

Arguments:
  account    The account [required]
  package    The package's name [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm owner offer

```text
Usage: cxpm owner offer [OPTIONS] account package

Offer a package to another account, which takes it with `cxpm owner take`

Arguments:
  account    The account [required]
  package    The package's name [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm owner take

```text
Usage: cxpm owner take [OPTIONS] package

Take a package offered to the account logged in

Arguments:
  package    The package's name [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm owner decline

```text
Usage: cxpm owner decline [OPTIONS] package

Withdraw an offer of a package, or decline one

Arguments:
  package    The package's name [required]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm dependency

```text
Usage: cxpm dependency [OPTIONS] COMMAND [ARGS]...

Add, remove and list the project's dependencies

Commands:
  add       Add a dependency to the manifest and fetch it: a package of the index, by name, or a
            repository, by its address
  remove    Remove a dependency from the manifest
  list      List the dependencies the manifest declares
  update    Move a dependency, or every one, to the newest version the manifest accepts, and pin it
            in .cxpm/lock.json
  sync      Fetch what the manifest declares and pin it in .cxpm/lock.json: `cxpm fetch`

Options:
  -h, --help  Show this message and exit.
```

## cxpm dependency add

```text
Usage: cxpm dependency add [OPTIONS] package [directory]

Add a dependency to the manifest and fetch it: a package of the index, by name, or a repository, by
its address

Arguments:
  package    name, @scope/name, name@constraint (fmt@>=10, fmt@10.2), or the address of a git
             repository, with #revision after it [required]
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help       Show this message and exit.
  --index TEXT     The package index to use instead of CXPM_INDEX or the public one; `off` for none
  --name TEXT      What a repository's package is called, when not the last part of its address
  --revision TEXT  The branch, tag or commit of a repository
  --no-sync        Only write the manifest: don't fetch
```

## cxpm dependency remove

```text
Usage: cxpm dependency remove [OPTIONS] name [directory]

Remove a dependency from the manifest

Arguments:
  name    The dependency's name [required]
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help  Show this message and exit.
```

## cxpm dependency list

```text
Usage: cxpm dependency list [OPTIONS] [directory]

List the dependencies the manifest declares

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help  Show this message and exit.
```

## cxpm dependency update

```text
Usage: cxpm dependency update [OPTIONS] [name] [directory]

Move a dependency, or every one, to the newest version the manifest accepts, and pin it in
.cxpm/lock.json

Arguments:
  name    The dependency (default: every one)
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm dependency sync

```text
Usage: cxpm dependency sync [OPTIONS] [directory]

Fetch what the manifest declares and pin it in .cxpm/lock.json: `cxpm fetch`

Arguments:
  directory    The project directory (default: .) [default: .]

Options:
  -h, --help    Show this message and exit.
  --index TEXT  The package index to use instead of CXPM_INDEX or the public one; `off` for none
```

## cxpm project

```text
Usage: cxpm project [OPTIONS] COMMAND [ARGS]...

Create projects

Commands:
  create    Create a project with a package.cpp and sources

Options:
  -h, --help  Show this message and exit.
```

## cxpm project create

```text
Usage: cxpm project create [OPTIONS] name

Create a project with a package.cpp and sources

Arguments:
  name    The project's name; also the directory it is created in [required]

Options:
  -h, --help   Show this message and exit.
  --type TEXT  executable, shared-library, static-library or header-library [default: executable]
  --tests      Also create a tests project for a library, run by `cxpm test`
  --example    Also create an example project that uses the library
```
