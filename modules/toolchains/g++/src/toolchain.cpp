#include <CXPM/Toolchain.hpp>

auto toolchain = CXPM::Models::Toolchain()
                     .name_set("g++")
                     .version_set("generic")
                     .include_directory_prefix_set("-I")
                     .link_directory_prefix_set("-L")
                     .link_library_prefix_set("-l")
                     .linker_options_set({"-fPIC", "-shared"})
                     .compiler_executable_set("/usr/bin/g++")
                     .linker_executable_set("/usr/bin/g++")
                     .archiver_executable_set("/usr/bin/ar")
                     .archiver_options_set({"rcs"})
                     .language_set("c++")
                     .object_build_options_set({"-fPIC"})
                     .compiler_options_set({"-fPIE", "-pthread"})
                     .object_prefix_set("")
                     .object_suffix_set(".o")
                     .shared_object_prefix_set("lib")
                     .shared_object_suffix_set(".so")
                     .archive_prefix_set("lib")
                     .archive_suffix_set(".a")
                     .executable_prefix_set("")
                     .executable_suffix_set("");

extern "C" Toolchain *get_toolchain() { return &toolchain; }