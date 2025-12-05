#include <CXPM/Toolchain.hpp>

auto toolchain = Models::Toolchain()
                     .name_set("gcc")
                     .version_set("generic")
                     .include_directory_prefix_set("-I")
                     .link_directory_prefix_set("-L")
                     .link_library_prefix_set("-l")
                     .compiler_executable_set("/usr/bin/gcc")
                     .linker_executable_set("/usr/bin/gcc")
                     .language_set("c")
                     .compiler_options_set({"-pthread"});

extern "C" Toolchain *get_toolchain() { return &toolchain; }