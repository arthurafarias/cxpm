#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/Status.hpp"

namespace CXPM::Models {

using BuildOutputResult = std::tuple<Status,
                                     BasicCollection<CompileCommandDescriptor>>;
} // namespace CXPM::Models