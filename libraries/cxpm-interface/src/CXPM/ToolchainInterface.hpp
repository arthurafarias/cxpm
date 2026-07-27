#pragma once

#include "CXPM/ToolchainBuildInterface.hpp"
#include "CXPM/ToolchainDescriptorInterface.hpp"
#include "CXPM/ToolchainInstallInterface.hpp"

namespace CXPM {
template <typename DerivedType>
struct ToolchainInterface : public ToolchainBuildInterface,
                            public ToolchainInstallInterface,
                            ToolchainDescriptorInterface<DerivedType> {};
} // namespace CXPM