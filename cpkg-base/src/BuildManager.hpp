#pragma once

#include "Package.hpp"
#include "Project.hpp"

#include <deque>

class BuildManager
{
    public:

    static void append(const Project& workspace) {

        for (const auto& dependency : workspace.targets) {
            dependencies.push_back(dependency);
        }

    }

    private:
    
    static inline std::deque<Package> dependencies;
};