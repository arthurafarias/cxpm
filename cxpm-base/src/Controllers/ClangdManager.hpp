#pragma once

#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>
#include <Models/TargetDescriptor.hpp>
#include <Modules/Serialization/JsonOutputArchiver.hpp>

#include <Models/CompilerCommandDescriptor.hpp>
#include <Models/PackageDescriptor.hpp>
#include <Utils/Macros/StaticClass.hpp>
#include <fstream>
#include <sstream>

using namespace Core::Containers;
using namespace Models;

namespace Controllers {

class ClangdManager final {

StaticClass(ClangdManager)

    public
    : static inline String
      emit_compiler_commands(const TargetDescriptor &target) {

    auto stream = std::ostringstream();
    Modules::Serialization::JsonOutputArchiver output(stream);
    Collection<CompileCommandDescriptor> compile_commands;
    output % compile_commands;
    return stream.str();
  }
};

} // namespace Controllers