#pragma once

#include "Models/ToolchainBuildInterface.hpp"
#include "Models/ToolchainInstallInterface.hpp"

namespace Models {
struct ToolchainInterface : public ToolchainBuildInterface,
                            public ToolchainInstallInterface {};
} // namespace Models