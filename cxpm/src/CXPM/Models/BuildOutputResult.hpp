#pragma once

#include <CXPM/Models/CompilerCommandDescriptor.hpp>

namespace CXPM::Models {
enum BuildOutputResultStatus { Success, Failure };

using BuildOutputResult = std::tuple<BuildOutputResultStatus,
                                     Collection<CompileCommandDescriptor>>;
} // namespace CXPM::Models