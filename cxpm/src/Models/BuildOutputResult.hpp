#pragma once

#include "Models/CompilerCommandDescriptor.hpp"

namespace Models {
enum BuildOutputResultStatus { Success, Failure };

using BuildOutputResult = std::tuple<BuildOutputResultStatus,
                                     Collection<CompileCommandDescriptor>>;
} // namespace Models