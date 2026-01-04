#pragma once

#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Views::Resources
{
    inline static const 
    String ApplicationViewHelpText = R"(
cxpm: A simple package manager written in C++ for C++
Usage:
    cxpm [option] [arguments]
Options:

    -b|--build <directory>: Build project on directory containing package.cpp
    -i|--install <directory>: Install project on directory containing package.cpp
    -u|--uninstall <directory>: Uninstall project on directory containing package.cpp

)";

}