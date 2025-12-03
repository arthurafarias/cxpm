# cppkg

Note: For Indra Mobility Friends, I invite you to see how to design something in C++ in this repository here. Maybe you should stop telling lies everywhere.

Because I don't like to search on stack overflow how to use every new function on other package manager for C++.

This is a simple project on a new package manager that don't rely on any declarative language or other than C++. It is heavely inspired in Julia, and Swift Languages. That uses the own language to declare packages.

So the structure by now is simple and I didn't investigate how to unroll dependency trees and whatever. It's just a concept and a simple implementation on what I think it should be.

Here is an example a project organized as:


```
- src
    - main.cpp
- package.cpp
```

would have a package.cpp as following

```cpp
#include "Models/TargetDescriptor.hpp"
#include <Models/ProjectDescriptor.hpp>

 auto example = Models::TargetDescriptor()
                       .name_set("example-executable")
                       .version_set("1.0.0")
                       .type_set("executable")
                       .sources_append({"src/main.cpp", "src/source0.cpp",
                                 "src/source1.cpp", "src/source2.cpp"})
                       .sources_append({"src/source3.cpp"})
                       .options_append({"-fPIE", "-fstack-protector-all"})
                       .link_libraries_append({"m"})
                       .create();

auto project = Models::ProjectDescriptor()
                        .add(example)
                        .create();
```

In my perspective, to make this work, the cpkg-build application would be responsible of creating the stubs to generate a shared-library, that would plug in the build system and compile the files.

I am trying to push the boundaries and modernize C++ in a way that can borrow new interesting perspective from modern Languages. I am designing it
like a minimal stuff that don't rely on complex macros and whatever. It's just one thing: Project and Packages should describe your project and package.cpp would be publicised declaring it. Nothing else. It's simple like package.json, Package.swift and Pkg.jl...

If you want to contribute or discuss this idea, just fork it and propose something!!

# How to use it?

The example directory contains the folder executable and it is organized as an executable.

To compile it using this concept, you should build this project using CMake.

```bash
mkdir build
cd build && cmake ..
sudo make install
```

In the build folder it will generate cpkg-build utility.

```
cd ..
cd examples/executable
cpkg-build --build .
```

The output should be like the following

```
g++ -fPIC -shared  -I/usr/share/cpkg-build/private-headers/ -Icpkg-build/src -Icpkg-base/src  -o package.cpp.o -c package.cpp
g++ -fPIC -shared  -I/usr/share/cpkg-build/private-headers/ -Icpkg-build/src -Icpkg-base/src  -o package.loader.cpp.o -c package.loader.cpp
g++ -fPIC -shared  -I/usr/share/cpkg-build/private-headers/ -Icpkg-build/src -Icpkg-base/src  -o project-manifest.so package.cpp.o package.loader.cpp.o
g++   -o src/main.cpp.o -c src/main.cpp
g++   -o example-executable src/main.cpp.o
```

# News

## Toolchain Support: You can define your own toolchain so you can cross compile to any target

```c++
#include <Models/ToolchainDescriptor.hpp>

using namespace Models;

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

## PkgConfig Support

Now, pkg-config dependencies are automatically solved in dependencies field from package

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
                   .dependencies_append("gstreamer-1.0") // here you define PkgConfig dependencies
                   .create();

auto project = Models::ProjectDescriptor().add(example).create();
```