#include <Controllers/Toolchain.hpp>
#include <Models/BasicToolchain.hpp>

using namespace Models;

auto toolchain = BasicToolchainFactory::instance()
                     .name("gcc")
                     .version("generic")
                     .compiler_executable("/usr/bin/gcc")
                     .linker_executable("/usr/bin/gcc")
                     .compiler_options({"-pthread"})
                     .build();

extern "C" ToolchainDescriptor *get_toolchain() { return &toolchain; }