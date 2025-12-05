#pragma once

#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/TargetDescriptor.hpp>
#include <CXPM/Modules/Serialization/JsonOutputArchiver.hpp>

#include <CXPM/CompilerCommandDescriptor.hpp>
#include <CXPM/PackageDescriptor.hpp>
#include <CXPM/Utils/Macros/StaticClass.hpp>

#include <sstream>

using namespace Core::Containers;
using namespace Models;

namespace Controllers {

class ClangdManager final {

StaticClass(ClangdManager)

    public : static inline String
             emit_compiler_commands(const TargetDescriptor &target) {

    auto stream = std::ostringstream();
    Modules::Serialization::JsonOutputArchiver output(stream);
    Collection<CompileCommandDescriptor> compile_commands;
    output % compile_commands;
    return stream.str();
  }
};

} // namespace Controllers