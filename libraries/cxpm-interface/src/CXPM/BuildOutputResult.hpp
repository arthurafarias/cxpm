#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/Status.hpp"

namespace Models {

using BuildOutputResult = std::tuple<Status,
                                     BasicCollection<CompileCommandDescriptor>>;
} // namespace Models