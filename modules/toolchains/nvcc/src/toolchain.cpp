#include <CXPM/Toolchain.hpp>

CXPM::ToolchainDescriptor toolchain = CXPM::Toolchain()
                     .name_set("nvcc")
                     .version_set("generic")
                     .include_directory_prefix_set("-I")
                     .link_directory_prefix_set("-L")
                     .link_library_prefix_set("-l")
                     .compiler_executable_set("/opt/cuda/bin/nvcc")
                     .linker_executable_set("/opt/cuda/bin/nvcc")
                     .language_set("cuda")
                     .compiler_options_set({"-pthread"});