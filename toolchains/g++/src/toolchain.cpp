#include <Models/Toolchain.hpp>

using namespace Models;

auto toolchain = Toolchain()
                     .name_set("g++")
                     .version_set("generic")
                     .include_directory_prefix_set("-I")
                     .link_directory_prefix_set("-L")
                     .link_library_prefix_set("-l")
                     .compiler_executable_set("/usr/bin/g++")
                     .linker_executable_set("/usr/bin/g++")
                     .archiver_executable_set("/usr/bin/ar")
                     .archiver_options_set({"rcs"})
                     .language_set("c++")
                     .compiler_options_set({"-pthread"})
                     .create();

extern "C" Toolchain *get_toolchain() { return &toolchain; }