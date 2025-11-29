#include <Controllers/Toolchain.hpp>
#include <Models/BasicToolchain.hpp>

using namespace Models;

auto toolchain = BasicToolchainFactory::instance()
                     .name("g++")
                     .version("generic")
                     .compiler_executable("/usr/bin/g++")
                     .linker_executable("/usr/bin/g++")
                     .language("c++")
                     .compiler_options({"-pthread"})
                     .build();

extern "C" ToolchainDescriptor *get_toolchain() { return &toolchain; }