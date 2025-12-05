#pragma once

#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/ProjectDescriptor.hpp"

namespace Models {
enum BuildOutputResultStatus { Success, Failure };

using BuildResult = std::tuple<BuildOutputResultStatus,
                                     Collection<CompileCommandDescriptor>>;
} // namespace Models