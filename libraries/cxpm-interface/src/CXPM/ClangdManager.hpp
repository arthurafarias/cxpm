#pragma once

#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/TargetDescriptor.hpp>
#include <CXPM/Modules/Serialization/JsonOutputArchiver.hpp>

#include <CXPM/CompilerCommandDescriptor.hpp>
#include <CXPM/PackageDescriptor.hpp>
#include <CXPM/Utils/Macros/StaticClass.hpp>

#include <sstream>

using namespace CXPM::Core::Containers;
using namespace CXPM;

// See docs/SRS-architecture.md item A17: consolidated onto CXPM::Controllers (matching
// ProjectManager/ToolchainManager/PackageConfigManager) instead of the separate global
// ::Controllers this used to live in. This header is still dead/never-included code with its
// own separate defect (item A14: Collection<T> is misused here as if it were a template) --
// that is unrelated to and unfixed by this namespace change.
namespace CXPM::Controllers {

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

} // namespace CXPM::Controllers