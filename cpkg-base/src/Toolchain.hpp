#pragma once

#include <Variant.hpp>

#include <deque>

class Toolchain;
class Toolchain
{
    public:
    Variant name;

    std::deque<Variant> include_directories;

    Variant compiler_executable;
    std::deque<Variant> compiler_options;

    Variant linker_executable;
    std::deque<Variant> linker_options;

    
    std::deque<Variant> link_directories;
};

inline static Toolchain DefaultToolchain = {
    .name = "GCC",
    .include_directories = {},

    .compiler_executable = "g++",
    .compiler_options = {},

    .linker_executable = "g++",
    .linker_options = {}
};