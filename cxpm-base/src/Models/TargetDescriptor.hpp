#pragma once

#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/PackageDescriptor.hpp"
#include <Modules/Serialization/Base/AbstractArchiver.hpp>
#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>

using namespace Core::Containers;
using namespace Modules::Serialization::Base;

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
Archiver &operator%(Archiver &ar, TargetDescriptor &td) {
  ar % TagFactory::make_object_start("TargetDescriptor");
  ar % TagFactory::make_named_value_property("name", td.name);
  ar % TagFactory::make_named_value_property("version", td.version);
  ar % TagFactory::make_named_value_property("description", td.description);
  ar % TagFactory::make_named_value_property("include_directories", td.include_directories);
  ar % TagFactory::make_named_value_property("link_directories", td.link_directories);
  ar % TagFactory::make_named_value_property("link_libraries", td.link_libraries);
  ar % TagFactory::make_named_value_property("options", td.options);
  ar % TagFactory::make_named_value_property("type", td.type);
  ar % TagFactory::make_named_value_property("url", td.url);
  ar % TagFactory::make_named_value_property("language", td.language);
  ar % TagFactory::make_named_value_property("toolchain", td.toolchain);
  ar % TagFactory::make_named_value_property("sources", td.sources);
  ar % TagFactory::make_named_value_property("dependencies", td.dependencies);
  ar % TagFactory::make_named_value_property("compile_commands", td.compile_commands);
  ar % TagFactory::make_named_value_property("install_prefix", td.install_prefix);
  ar % TagFactory::make_object_end("TargetDescriptor");
  return ar;
}

} // namespace Models