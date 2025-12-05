#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/PackageDescriptor.hpp"
#include <CXPM/Modules/Serialization/AbstractArchiver.hpp>
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>

using namespace Core::Containers;
using namespace Modules::Serialization;

namespace Models {
struct TargetDescriptor : public PackageDescriptor {
  String type = "executable";
  String url = "";
  String language = "c++";
  String description = "";
  String toolchain = "g++";
  Collection<String> sources;
  Collection<String> dependencies;
  Collection<CompileCommandDescriptor> compile_commands;
};

/** How C++ STL makers plans to create reflections in the language? */
template <typename Archiver>
Archiver &operator%(Archiver &ar, const TargetDescriptor &td) {
  ar % AbstractArchiver::make_object_start("TargetInterface");
  ar % AbstractArchiver::make_named_value_property("name", td);
  ar % AbstractArchiver::make_named_value_property("version", td.version);
  ar % AbstractArchiver::make_named_value_property("description", td.description);
  ar % AbstractArchiver::make_named_value_property("include_directories", td.include_directories);
  ar % AbstractArchiver::make_named_value_property("link_directories", td.link_directories);
  ar % AbstractArchiver::make_named_value_property("link_libraries", td.link_libraries);
  ar % AbstractArchiver::make_named_value_property("options", td.options);
  ar % AbstractArchiver::make_named_value_property("type", td.type);
  ar % AbstractArchiver::make_named_value_property("url", td.url);
  ar % AbstractArchiver::make_named_value_property("language", td.language);
  ar % AbstractArchiver::make_named_value_property("toolchain", td.toolchain);
  ar % AbstractArchiver::make_named_value_property("sources", td.sources);
  ar % AbstractArchiver::make_named_value_property("dependencies", td.dependencies);
  ar % AbstractArchiver::make_named_value_property("compile_commands", td.compile_commands);
  ar % AbstractArchiver::make_named_value_property("install_prefix", td.install_prefix);
  ar % AbstractArchiver::make_object_end("TargetInterface");
  return ar;
}

} // namespace Models