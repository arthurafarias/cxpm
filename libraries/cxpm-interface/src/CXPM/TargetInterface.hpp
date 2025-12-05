#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/PackageInterface.hpp"
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Modules/Serialization/AbstractArchiver.hpp>

using namespace Core::Containers;
using namespace Modules::Serialization;

namespace Models {

template <typename DerivedType>
struct TargetInterface : public PackageInterface<DerivedType> {
  virtual const String &type_get() const = 0;
  virtual DerivedType &type_set(const String &) = 0;
  virtual const String &url_get() const = 0;
  virtual DerivedType &url_set(const String &) = 0;
  virtual const String &language_get() const = 0;
  virtual DerivedType &language_set(const String &) = 0;
  virtual const String &description_get() const = 0;
  virtual DerivedType &description_set(const String &) = 0;
  virtual const String &toolchain_get() const = 0;
  virtual DerivedType &toolchain_set(const String &) = 0;
  virtual const Collection<String> &sources_get() const = 0;
  virtual DerivedType &sources_set(const Collection<String> &) = 0;
  virtual const Collection<String> &dependencies_get() const = 0;
  virtual DerivedType &dependencies_set(const Collection<String> &) = 0;
  virtual const Collection<CompileCommandDescriptor> &
  compile_commands_get() const = 0;
  virtual DerivedType &
  compile_commands_set(const Collection<CompileCommandDescriptor> &) = 0;
};

/** How C++ STL makers plans to create reflections in the language? */
template <typename Archiver, typename DerivedType>
Archiver &operator%(Archiver &ar, const TargetInterface<DerivedType> &td) {
  ar % AbstractArchiver::make_object_start("TargetInterface");
  ar % AbstractArchiver::make_named_value_property(
           "name", [&td](auto &v) { td.type_set(v); },
           [&td]() { return td.type_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "version", [&td](auto &v) { td.version_set(v); },
           [&td]() { return td.version_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "description", [&td](auto &v) { td.description_set(v); },
           [&td]() { return td.description_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "include_directories",
           [&td](auto &v) { td.include_directories_set(v); },
           [&td]() { return td.include_directories_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "link_directories", [&td](auto &v) { td.link_directories_set(v); },
           [&td]() { return td.link_directories_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "link_libraries", [&td](auto &v) { td.link_libraries_set(v); },
           [&td]() { return td.link_libraries_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "options", [&td](auto &v) { td.options_set(v); },
           [&td]() { return td.options_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "type", [&td](auto &v) { td.type_set(v); },
           [&td]() { return td.type_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "url", [&td](auto &v) { td.url_set(v); },
           [&td]() { return td.url_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "language", [&td](auto &v) { td.language_set(v); },
           [&td]() { return td.language_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "toolchain", [&td](auto &v) { td.toolchain_set(v); },
           [&td]() { return td.toolchain_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "sources", [&td](auto &v) { td.sources_set(v); },
           [&td]() { return td.sources_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "dependencies", [&td](auto &v) { td.dependencies_set(v); },
           [&td]() { return td.dependencies_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "compile_commands", [&td](auto &v) { td.compile_commands_set(v); },
           [&td]() { return td.compile_commands_get(); });
  ar % AbstractArchiver::make_named_value_property(
           "install_prefix", [&td](auto &v) { td.install_prefix_set(v); },
           [&td]() { return td.install_prefix_get(); });
  ar % AbstractArchiver::make_object_end("TargetInterface");
  return ar;
}

} // namespace Models