#pragma once

#include "Package.hpp"
#include <Variant.hpp>
#include <deque>
#include <ostream>


#include <Utils/Print.hpp>

struct Project
{
    public:
    const std::deque<Package> targets;
};

template<>
inline std::ostream& print<Project>(std::ostream& os, Project project)
{
    print(os, "{");
    
    for (auto pkg : project.targets) {
        print(os, pkg);
    }
    
    print(os, "}");

    return os;
}