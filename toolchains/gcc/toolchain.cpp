#include <Models/BasicToolchain.hpp>
#include <Controllers/Toolchain.hpp>

using namespace Models;

BasicToolchain cpkg_toolchain_gcc_generic = {
    .name = "gcc",
    .version = "generic",
    .compiler_executable = "g++",
    .compiler_options = {"-pthread"},
    .linker_executable = "g++",
};

extern "C" BasicToolchain* get_toolchain() { return &cpkg_toolchain_gcc_generic; }