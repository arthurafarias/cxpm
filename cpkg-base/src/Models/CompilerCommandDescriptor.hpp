#pragma once

#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Models {
struct CompileCommandDescriptor {
  String directory;
  String command;
  String file;
  String output;
  
  String stdout;
  String stderr;
};

template <typename Archiver>
inline Archiver &operator%(Archiver &ar,
                           const CompileCommandDescriptor &descriptor) {

  // ar % Archiver::make_object_start<CompileCommandDescriptor>("CompilerCommandDescriptor");
  ar % Archiver::make_named_value_property("directory", descriptor.directory);
  ar % Archiver::make_named_value_property("command", descriptor.command);
  ar % Archiver::make_named_value_property("file", descriptor.file);
  ar % Archiver::make_named_value_property("output", descriptor.output);
  // ar % Archiver::make_object_end<CompileCommandDescriptor>("CompilerCommandDescriptor");

  return ar;
}
} // namespace Models