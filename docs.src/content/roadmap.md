---
title: Roadmap
---

# Roadmap &amp; suggested improvements

This page collects concrete next steps, roughly ordered by leverage: cheap/safe fixes first,
then correctness-affecting design decisions, then longer-range architecture work. Every item
with an "A#" reference is itemized in detail, with file:line citations, in
[srs-architecture.md](srs-architecture.md).

## Recently shipped

Three capabilities landed together and are documented as their own SRS pages rather than roadmap
items: [package.json/toolchain.json as manifest alternatives](srs-json-manifests.md),
[`cxpm --generate`](srs-generate.md), and [sandboxed descriptor extraction](srs-sandbox.md). The
JSON manifest path also directly mitigates part of item 6 below (no JIT-compilation cost at all
on that path) and the sandbox directly mitigates the highest-severity concern that would otherwise
apply to item 8 below (prebuilt/cached plugin artifacts would otherwise be `dlopen`'d with no
isolation).

## Near-term (cheap, safe, high value)

1. **Add CI** (A36). No workflow file exists in the repository today. At minimum, run
   `ctest -L unit` (no compiler dependency beyond the build itself) on every push/PR; a second,
   slower job with a real compiler on `PATH` can additionally run
   `ctest -L integration -DCXPM_BUILD_CLI_INTEGRATION_TESTS=ON`. This is the single highest-leverage
   item on this list: it turns every other item below from "fixed once" into "stays fixed."
2. **Wire up or remove `--uninstall`** (A13). It's advertised in `--help` today but does nothing.
   Either implement it (the reverse of `install_target`'s file copying) or drop it from the usage
   text until it exists — a documented flag that silently no-ops is worse than no flag.
3. ~~Fix the README/example inconsistencies (A30, A31)~~ — **done**: the flagship README example
   now uses `Target`/`Project`, and `Target::dependencies_append` was added for parity with every
   other collection field.
4. **Rename `ProjectDescriptor::compile_comands`** (A32) — a one-line fix with no behavioral risk.
   (The misleading `"package.json"` error message this item used to also list, A34, is now moot:
   `ProjectManager::build_project`'s old error-handling code was superseded wholesale by
   `load_project` when [package.json support](srs-json-manifests.md) was added, and that
   reference is correct now that `package.json` is a real manifest format.)
5. **Delete or finish the dead orphan headers**: `ClangdManager.hpp`, `ShellCommandBuilder.hpp`,
   `Modules/Serialization/KeyValueMapDescriptor.hpp` (A14), `Utils/Unix/which.hpp` (A15),
   `Core::{SharedPointer,UniquePointer,WeakPointer}` and `Utils/Print.hpp` (A16). None of these
   are reachable from any compiling code path today; keeping them either working or gone is
   strictly better than keeping them broken-and-included-in-nobody's-mental-model-of-the-API.

## Medium-term (design decisions, each independently shippable)

6. **Cache toolchain/manifest JIT compilation** (A21). Every `cxpm --build`/`--install` currently
   recompiles all discovered toolchain plugins *and* the project manifest from source, with no
   mtime/hash check. On this machine, a single `cxpm --build` against `examples/executable` spent
   roughly 12 of its ~13 total seconds recompiling three toolchain `.so` files it didn't need to
   recompile. A source-mtime (or content-hash) cache keyed by the generated `.so` path would cut
   the common case to milliseconds without changing the plugin model.
7. **Resolve the toolchain-plugin naming collision** (A22). In the current source-tree layout,
   `gcc/src/toolchain.cpp`, `g++/src/toolchain.cpp` and `nvcc/src/toolchain.cpp` all derive the
   same directory-based plugin name (`"src"`), so they overwrite each other's generated
   `.loader.cpp`/`.so` mid-scan. It works today only because each plugin fully compiles, loads and
   unloads before the next one starts — fixing the naming (derive it from `.name_get()` instead
   of the parent directory) removes a foot-gun that a future contributor could trip on by
   restructuring the module layout in a way that seems harmless.
8. **Pick one toolchain deployment mechanism** (A29). `build.sh` deploys prebuilt `.so` files;
   `ToolchainManager::autoscan` only ever looks for `toolchain.cpp` source files. Given item 6
   above, a prebuilt-`.so`-with-cache-invalidation model is probably the right end state — decide
   and delete the other path.
9. ~~Consolidate the two `Controllers` namespaces (A17)~~ — **done**, and not a moment too soon:
   adding `CXPM::Controllers::DescriptorSandbox` (see [srs-sandbox.md](srs-sandbox.md)) flipped
   which `Controllers` namespace unqualified lookup found first and turned this from a latent
   footgun into an actual build failure. `PkgConfigManager`/`ClangdManager` now live in
   `CXPM::Controllers` too. The still-open half of this item is adding the missing `CXPM::`
   prefix on `Tuple`/`KeyValueMapDescriptor` (A18) — a latent footgun, not an active bug, so it
   remains independently schedulable.
10. **Decide `Toolchain::install`'s fate** (A10). Either implement it and route the CLI through it
    (unifying it with `ProjectManager::install_target`'s separate, hand-rolled copying logic), or
    remove `install()` from `ToolchainInstallInterface` entirely. Leaving a pure-virtual interface
    method that every toolchain plugin must nominally satisfy, but that always throws and is never
    called, is confusing for anyone implementing a new toolchain.
11. **Replace `popen`+string-concatenation with `posix_spawn`/argv-array execution** (A28). This is
    the most consequential correctness item on this list from a security standpoint: target names,
    source paths and `pkg-config` output currently flow unescaped into shell command lines built
    via string concatenation, and `ShellManager::exec(cmd, dry, shell=true)` double-wraps through
    `bash -c` *and* `popen`'s own implicit `/bin/sh -c`. This is worth fixing before cxpm is used
    against untrusted or generated `package.cpp`/dependency data.
12. **Fix `Lockable`'s copy semantics or remove it** (A24, A25). Every copy currently gets a fresh
    mutex, and no `BasicCollection` operation actually acquires the lock it drags in — so it
    provides the appearance of thread-safety without the substance. Decide whether
    `BasicCollection`/`Object`/CLI-level concurrent access is an actual requirement; if not, delete
    `Lockable` from the inheritance chain rather than leave it as a half-finished primitive.

## Longer-range

13. **A real dependency-resolution story.** The README already flags this as unexplored
    ("many advanced features, such as full dependency graph resolution, are still under
    exploration"). `Target::dependencies_get/set` exists as a flat string list today with no
    resolution, version constraints, or lock file. A minimal version-pinned resolver against
    pkg-config-discoverable packages (building on `PackageConfigManager::find_package`) would be a
    natural first increment.
14. ~~An input archiver / manifest introspection story.~~ — **done**: `AbstractInputArchiver`'s
    constructor access-specifier bug (A33) is fixed, and `JsonInputArchiver` plus the full
    `package.json`/`toolchain.json` manifest path (see [srs-json-manifests.md](srs-json-manifests.md))
    now let tooling introspect or author a project with no compiler in the loop.
15. **ABI/version-checked plugin loading** (A23). Both `ProjectManager` and `ToolchainManager`
    `dlopen` a freshly-compiled `.so` and trust its layout matches the headers `cxpm` itself was
    built against, with no explicit version check. The sandbox ([srs-sandbox.md](srs-sandbox.md))
    confines a layout mismatch's blast radius to the disposable child process, but does not add
    the version check itself — that's still open, and worth landing *before* or *alongside* item 6
    or item 8's caching work, since a mismatch becomes much more likely once a cached/prebuilt
    artifact can outlive the header tree it was compiled against.
16. **A `find_package(cxpm-interface)`-consumable install.** `cxpm-interface`'s CMake target
    installs headers but exports nothing (no `install(EXPORT ...)`, no
    `CxpmInterfaceConfig.cmake`), so downstream CMake projects can only consume it via raw
    include-path wiring, the same way `build.sh` and the JIT-compiled plugins already do in-tree.
    A proper CMake package export would let external toolchain-plugin authors depend on
    `cxpm-interface` the normal CMake way instead of guessing include paths.
17. **Stronger sandbox isolation.** [srs-sandbox.md §4](srs-sandbox.md#4-known-limitations-accepted-for-this-iteration)
    bounds CPU/memory/wall-clock time and removes direct memory access to the parent process, but
    does not restrict filesystem or network access from within the sandboxed child. Linux
    `seccomp-bpf` and/or a mount/user namespace would meaningfully narrow that further, at the cost
    of platform-specific code the current implementation deliberately avoided for portability.
18. **Automate the `CXPM_SANDBOX_DISABLE=1` fallback path as a CTest case.** Currently verified
    manually only (see [srs-sandbox.md §5](srs-sandbox.md#5-testing)); promoting it to an
    integration test would close the one deliberately-manual gap in that SRS's test coverage.
19. **Unit-test `ToolchainManager`/`ProjectManager` orchestration against a fake toolchain.**
    `ToolchainInterface` was already pure-virtual and mockable before this session; the sandbox
    work adds another natural seam (`DescriptorSandbox` could be made injectable rather than
    called directly) that would let `build_project`'s toolchain-selection and error-handling logic
    be unit-tested without a real compiler — see [srs-unit-testing.md §8](srs-unit-testing.md#8-deferred-coverage--future-work),
    which this item would close.

## What was intentionally *not* changed in this pass

The remaining open items above are design decisions with real trade-offs (performance vs.
plugin-model simplicity, security hardening vs. implementation effort, concurrency guarantees vs.
API surface). They are documented here and in [srs-architecture.md](srs-architecture.md) with
enough detail (file:line citations, concrete correction sketches) to be picked up independently,
rather than bundled into this change — see [srs-architecture.md §4.2](srs-architecture.md#42-deferred-itemized-not-changed-in-this-pass)
for the full itemized list of still-deferred findings with severities, and
[srs-sandbox.md §4](srs-sandbox.md#4-known-limitations-accepted-for-this-iteration) for the
sandbox's own explicitly accepted limitations.
