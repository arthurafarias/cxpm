#pragma once

#include "Models/TargetDescriptor.hpp"
#include "Serialization/AbstractArchiver.hpp"
#include "Serialization/JsonOutputArchiver.hpp"
#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>

#include <Models/PackageDescriptor.hpp>
#include <Utils/Macros/StaticClass.hpp>
#include <type_traits>

using namespace Core::Containers;
using namespace Models;

namespace Controllers {

struct CompileCommandDescriptor {
  String directory;
  String command;
  String file;
  String output;
};

class ClangdManager final {

StaticClass(ClangdManager)

    public : static inline String
             emit_compiler_commands(const TargetDescriptor &target) {

    Serialization::JsonOutputArchiver output("compile_commands.json");
    CompileCommandDescriptor descriptor{};
    Collection<CompileCommandDescriptor> compile_commands;
    output % compile_commands;
    auto result = output.to_string();
    return result;
  }
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar,
                           const CompileCommandDescriptor &descriptor) {
  ar % Serialization::ElementDescriptor::ObjectStart;
  ar % Archiver::make_named_value_property("directory", descriptor.directory);
  ar % Archiver::make_named_value_property("command", descriptor.command);
  ar % Archiver::make_named_value_property("file", descriptor.file);
  ar % Archiver::make_named_value_property("output", descriptor.output);
  ar % Serialization::ElementDescriptor::ObjectEnd;
  return ar;
}
} // namespace Controllers