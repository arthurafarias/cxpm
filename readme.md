# cppkg

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
#include <Models/BasicProject.hpp>
#include <Models/BasicTarget.hpp>

auto descriptor = Models::BasicTargetFactory::create()
                      .name("example-executable")
                      .version("1.0.0")
                      .type("executable")
                      .sources({"src/main.cpp", "src/source0.cpp",
                                "src/source1.cpp", "src/source2.cpp"})
                      .sources({"src/source3.cpp"})
                      .build();

// Models::BasicProject project = {.targets = {descriptor}};
auto project = Models::BasicProjectFactory::create().add(descriptor).build();
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
```

In the build folder it will generate cpkg-build utility.

```
cd ..
cd examples/executable
../../build/cpkg-build/cpkg-build --build .
```

The output should be like the following

```
g++ -fPIC -shared  -I/usr/share/cpkg-build/private-headers/ -Icpkg-build/src -Icpkg-base/src  -o package.cpp.o -c package.cpp
g++ -fPIC -shared  -I/usr/share/cpkg-build/private-headers/ -Icpkg-build/src -Icpkg-base/src  -o package.loader.cpp.o -c package.loader.cpp
g++ -fPIC -shared  -I/usr/share/cpkg-build/private-headers/ -Icpkg-build/src -Icpkg-base/src  -o project-manifest.so package.cpp.o package.loader.cpp.o
g++   -o src/main.cpp.o -c src/main.cpp
g++   -o example-executable src/main.cpp.o
```