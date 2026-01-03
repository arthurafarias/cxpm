![cxpm](misc/images/logo-banner-path.svg)

cxpm is an experimental package manager for C++ that aims to simplify project configuration by using C++ itself as the declarative language. Instead of relying on separate configuration formats, cxpm allows projects and packages to describe themselves directly in standard C++ code.

The design is inspired by modern ecosystems such as **Julia**, **Swift**, and their language-native package declarations.

This project is at an early conceptual stage: the architecture is intentionally minimal, and many advanced features (such as full dependency graph resolution) are still under exploration. The current implementation demonstrates how such a system could function in practice.

If you're interested in discussing or contributing to this idea, feel free to fork the repository and open a proposal.

# Project Structure Example

```
- src
    - main.cpp
- package.cpp
```

would have a package.cpp as following

```cpp
#include <CXPM/Models/TargetDescriptor.hpp>
#include <CXPM/Models/ProjectDescriptor.hpp>

 auto example = Models::TargetDescriptor()
                       .name_set("example-executable")
                       .version_set("1.0.0")
                       .type_set("executable")
                       .sources_append({"src/main.cpp"})
                       .options_append({"-fPIE", "-fstack-protector-all"})
                       .link_libraries_append({"m"})
                       .create();

auto project = Models::ProjectDescriptor()
                        .add(example)
                        .create();
```

The **cxpm** utility is responsible for generating the supporting stubs required to build a shared library representing your project’s manifest, integrating it seamlessly into the build process.

The overarching philosophy is to maintain a small, modern, and expressive system: no complex macro layers, no special DSLs. Your C++ project is described in C++, much like package.json, Package.swift, or Pkg.jl but for C++.

# Building and Running

The examples directory contains an example executable project.

To build and install the core tools:

```bash
git clone https://github.com/arthurafarias/cxpm
cd cxpm
./build.sh
sudo ./install.sh
```

This installs the cxpm utility.

You can now build an example project:

```
cd ../examples/executable
cxpm --build .
```

# Recent Enhancements

## Custom Toolchain Support

cxpm now supports custom toolchain definitions, enabling cross-compilation or integration with non-system compilers:

```c++
#include <CXPM/Models/ToolchainDescriptor.hpp>

using namespace CXPM::Models;

auto toolchain = Toolchain()
                     .name_set("g++")
                     .version_set("generic")
                     .include_directory_prefix_set("-I")
                     .link_directory_prefix_set("-L")
                     .link_library_prefix_set("-l")
                     .compiler_executable_set("/usr/bin/g++")
                     .linker_executable_set("/usr/bin/g++")
                     .language_set("c++")
                     .compiler_options_set({"-pthread"})
                     .create();

extern "C" Toolchain *get_toolchain() { return &toolchain; }
```

## Pkg-Config Integration

Pkg-config dependencies are now resolved automatically when specified in the package descriptor:

```c++
auto example = Models::TargetDescriptor()
                   .name_set("example-executable")
                   .version_set("1.0.0")
                   .type_set("executable")
                   .sources_append({"src/main.cpp", "src/source0.cpp",
                                    "src/source1.cpp", "src/source2.cpp"})
                   .sources_append({"src/source3.cpp"})
                   .options_append({"-fPIE", "-fstack-protector-all"})
                   .link_libraries_append({"m"})
                   .include_directories_append({})
                   .dependencies_append("gstreamer-1.0") // pkg-config dependency
                   .create();

auto project = Models::ProjectDescriptor().add(example).create();
```

## Installation Support

Projects can now be installed directly:

```c++
cxpm --install . --prefix /usr/local
```

cxpm follows a predictable, POSIX-aligned installation layout:

- Shared libraries → `<prefix>/lib`
- Static libraries → `<prefix>/lib`
- Executables → `<prefix>/bin`
- Headers → `<prefix>/include/<project-name>`
- pkg-config files → `<prefix>/lib/pkgconfig/<project-name>.pc`

The uniform structure avoids special-case install paths and ensures consistent behavior across environments.

# Contributing

**cxpm is an early-stage experimental project, and community participation is highly encouraged.**

If you are interested in modernizing the C++ tooling ecosystem, exploring language-native package definitions, or helping shape the direction of this concept, we would be happy to have your input.

You can contribute in several ways:

- Propose ideas or improvements
- Report issues or inconsistencies
- Submit pull requests
- Discuss design decisions or future directions

Whether you want to help refine the architecture, expand features, or simply share feedback, your participation is welcome.

**Feel free to open a discussion, file an issue, or fork the repository and start experimenting.** Together we can explore what a simpler, more expressive C++ package manager might look like.

Arthur Farias <afarias.arthur@gmail.com>
