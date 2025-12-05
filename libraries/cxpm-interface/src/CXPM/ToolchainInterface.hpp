#pragma once

#include "CXPM/ToolchainBuildInterface.hpp"
#include "CXPM/ToolchainDescriptorInterface.hpp"
#include "CXPM/ToolchainInstallInterface.hpp"

namespace Models {
template <typename DerivedType>
struct ToolchainInterface : public ToolchainBuildInterface,
                            public ToolchainInstallInterface,
                            ToolchainDescriptorInterface<DerivedType> {};
} // namespace Models