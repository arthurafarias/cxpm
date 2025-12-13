#pragma once

#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>
#include <Models/TargetDescriptor.hpp>
#include <Modules/Serialization/JSON/JSONOutputArchiver.hpp>

#include <Models/CompilerCommandDescriptor.hpp>
#include <Models/PackageDescriptor.hpp>
#include <Utils/Macros/StaticClass.hpp>
#include <fstream>
#include <sstream>

using namespace Core::Containers;
using namespace Models;
using namespace Modules::Serialization::JSON;

namespace Controllers {

class ClangdManager final {

StaticClass(ClangdManager)

    public
    : static inline String
      emit_compiler_commands(TargetDescriptor &target) {

    auto stream = std::ostringstream();
    JSONOutputArchiver output(stream);
    Collection<CompileCommandDescriptor> compile_commands;
    output << compile_commands;
    return stream.str();
  }
};

} // namespace Controllers