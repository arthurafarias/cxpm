#pragma once

#include "Modules/Serialization/Base/AbstractArchiver.hpp"
#include <Core/Containers/String.hpp>

using namespace Core::Containers;
using namespace Modules::Serialization::Base;

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

  ar % ArchiveTagFactory::make_object_start("CompilerCommandDescriptor");
  ar % ArchiveTagFactory::make_named_value_property("directory",
                                                    descriptor.directory);
  ar % ArchiveTagFactory::make_named_value_property("command",
                                                    descriptor.command);
  ar % ArchiveTagFactory::make_named_value_property("file", descriptor.file);
  ar %
      ArchiveTagFactory::make_named_value_property("output", descriptor.output);
  ar % ArchiveTagFactory::make_object_end("CompilerCommandDescriptor");

  return ar;
}
} // namespace Models