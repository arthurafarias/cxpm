#pragma once

#include <CXPM/Utils/Unused.hpp>
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Models/TargetDescriptor.hpp>
#include <CXPM/Modules/Serialization/JsonOutputArchiver.hpp>

#include <CXPM/Models/CompilerCommandDescriptor.hpp>
#include <CXPM/Models/PackageDescriptor.hpp>
#include <CXPM/Utils/Macros/StaticClass.hpp>
#include <sstream>

using namespace CXPM::Core::Containers;
using namespace CXPM::Models;

namespace CXPM::Controllers {

class ClangdManager final {

StaticClass(ClangdManager)

    public : static inline String
             emit_compiler_commands(const TargetDescriptor &target) {
    Utils::Unused{target};
    auto stream = std::ostringstream();
    Modules::Serialization::JsonOutputArchiver output(stream);
    Collection<CompileCommandDescriptor> compile_commands;
    output % compile_commands;
    return stream.str();
  }
};

} // namespace CXPM::Controllers