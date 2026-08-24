---
layout: default
title: cxpm
description: An experimental C++-native package manager and build tool for C++.
---

<div class="hero">
<h1>Describe your C++ project in C++.</h1>
<p><strong>cxpm</strong> is an experimental package manager and build tool for C++ that uses C++ itself, compiled and executed at build time, as the project-description language, instead of a separate configuration format.</p>
</div>

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
                   .create();

auto project = Project().add(example).create();
```

```bash
cxpm --generate package-json .   # or: cxpm --generate package-cpp .
cxpm --build .
cxpm --install . --prefix /usr/local
```

<div class="cards">
<div class="card"><h3>Two ways to describe a project</h3><p><code>package.cpp</code> is compiled into a shared object and loaded back through a generated loader stub — a real C++ translation unit, not a parsed format. <code>package.json</code> describes the same <code>ProjectDescriptor</code> declaratively: no compilation, no code execution at all.</p></div>
<div class="card"><h3>Pluggable toolchains</h3><p>Compiler/linker/archiver conventions are themselves described as a <code>ToolchainDescriptor</code>, either JIT-compiled from a <code>toolchain.cpp</code> plugin or read directly from a <code>toolchain.json</code>.</p></div>
<div class="card"><h3>Sandboxed by default</h3><p>Loading a compiled <code>package.cpp</code>/<code>toolchain.cpp</code> now happens in a resource-limited child process, not the main <code>cxpm</code> process — see the sandbox SRS below.</p></div>
<div class="card"><h3>POSIX-aligned install layout</h3><p>Shared/static libraries → <code>lib</code>, executables → <code>bin</code>, headers → <code>include/&lt;project&gt;</code>, pkg-config files → <code>lib/pkgconfig</code>.</p></div>
<div class="card"><h3>Early stage, audited honestly</h3><p>cxpm is an early conceptual prototype. The architecture SRS below itemizes real, cited flaws in the current implementation rather than glossing over them.</p></div>
</div>

## Project status

cxpm is at an early conceptual stage. This documentation set was written together with the
project's first test suite, so it doubles as an honest audit of what currently works, what is
stubbed or dead code, and what a contributor should fix first — see the
[architecture SRS](SRS-architecture.md) for the itemized list.

## Start here

- [Architecture](architecture.md)
- [Command-line tool (`cxpm`)](tooling.md)
- [Testing strategy](testing.md)
- [Software Requirements Specification: Unit Testing the API](SRS-unit-testing.md)
- [Software Requirements Specification: Current Architecture &amp; Flaws](SRS-architecture.md)
- [Software Requirements Specification: CMake/CTest Tooling Facility](SRS-ctest-tooling.md)
- [Software Requirements Specification: JSON Manifests](SRS-json-manifests.md)
- [Software Requirements Specification: `cxpm --generate`](SRS-generate.md)
- [Software Requirements Specification: Descriptor Sandbox](SRS-sandbox.md)
- [Roadmap &amp; suggested improvements](roadmap.md)
