#pragma once

// Descriptor-level JSON (de)serialization built on top of the generic Value <-> JSON codec
// (JsonValueParser.hpp/JsonValueWriter.hpp). This is what lets package.json/toolchain.json exist
// as serialization alternatives to package.cpp/toolchain.cpp: parse_json() -> a Value tree ->
// {target,toolchain,project}_descriptor_from_json() -> the same descriptor structs the compiled
// .cpp path produces. See docs/SRS-json-manifests.md.

#include "CXPM/Core/Exceptions/JsonParseException.hpp"
#include "CXPM/Modules/Serialization/JsonValueParser.hpp"
#include "CXPM/Modules/Serialization/JsonValueWriter.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/ToolchainDescriptor.hpp"

namespace CXPM::Modules::Serialization {

using CXPM::Core::Containers::BasicCollection;
using CXPM::Core::Containers::Map;
using CXPM::Core::Containers::String;
using CXPM::Core::Containers::Value;

inline Value string_collection_to_json(const BasicCollection<String> &collection) {
  BasicCollection<Value> array;
  for (const auto &item : collection) {
    array.push_back(Value(item));
  }
  return Value(array);
}

inline BasicCollection<String> string_collection_from_json(const Value &value) {
  if (!std::holds_alternative<BasicCollection<Value>>(value)) {
    throw CXPM::Core::Exceptions::JsonParseException(
        "expected a JSON array of strings");
  }
  BasicCollection<String> result;
  for (const auto &element : std::get<BasicCollection<Value>>(value)) {
    if (!std::holds_alternative<String>(element)) {
      throw CXPM::Core::Exceptions::JsonParseException(
          "expected a JSON string inside array");
    }
    result.push_back(std::get<String>(element));
  }
  return result;
}

inline const Map<String, Value> &as_json_object(const Value &value,
                                                 const char *what) {
  if (!std::holds_alternative<Map<String, Value>>(value)) {
    throw CXPM::Core::Exceptions::JsonParseException(
        "{} must be a JSON object", what);
  }
  return std::get<Map<String, Value>>(value);
}

inline String json_string_field(const Map<String, Value> &object,
                                const String &key,
                                const String &default_value) {
  auto it = object.find(key);
  if (it == object.end()) {
    return default_value;
  }
  if (!std::holds_alternative<String>(it->second)) {
    throw CXPM::Core::Exceptions::JsonParseException(
        "field '{}' must be a JSON string", key);
  }
  return std::get<String>(it->second);
}

inline BasicCollection<String>
json_string_array_field(const Map<String, Value> &object, const String &key) {
  auto it = object.find(key);
  if (it == object.end()) {
    return BasicCollection<String>();
  }
  return string_collection_from_json(it->second);
}

// --- TargetDescriptor ---------------------------------------------------------------------

inline Value to_json(const CXPM::TargetDescriptor &target) {
  Map<String, Value> object;
  object["name"] = Value(target.name);
  object["version"] = Value(target.version);
  object["install_prefix"] = Value(target.install_prefix);
  object["type"] = Value(target.type);
  object["url"] = Value(target.url);
  object["language"] = Value(target.language);
  object["description"] = Value(target.description);
  object["toolchain"] = Value(target.toolchain);
  object["sources"] = string_collection_to_json(target.sources);
  object["dependencies"] = string_collection_to_json(target.dependencies);
  object["include_directories"] =
      string_collection_to_json(target.include_directories);
  object["link_directories"] =
      string_collection_to_json(target.link_directories);
  object["link_libraries"] = string_collection_to_json(target.link_libraries);
  object["options"] = string_collection_to_json(target.options);
  return Value(object);
}

inline CXPM::TargetDescriptor target_descriptor_from_json(const Value &value) {
  const auto &object = as_json_object(value, "a target descriptor");
  CXPM::TargetDescriptor descriptor;

  descriptor.name = json_string_field(object, "name", descriptor.name);
  descriptor.version = json_string_field(object, "version", descriptor.version);
  descriptor.install_prefix =
      json_string_field(object, "install_prefix", descriptor.install_prefix);
  descriptor.type = json_string_field(object, "type", descriptor.type);
  descriptor.url = json_string_field(object, "url", descriptor.url);
  descriptor.language =
      json_string_field(object, "language", descriptor.language);
  descriptor.description =
      json_string_field(object, "description", descriptor.description);
  descriptor.toolchain =
      json_string_field(object, "toolchain", descriptor.toolchain);
  descriptor.sources = json_string_array_field(object, "sources");
  descriptor.dependencies = json_string_array_field(object, "dependencies");
  descriptor.include_directories =
      json_string_array_field(object, "include_directories");
  descriptor.link_directories =
      json_string_array_field(object, "link_directories");
  descriptor.link_libraries = json_string_array_field(object, "link_libraries");
  descriptor.options = json_string_array_field(object, "options");

  return descriptor;
}

// --- ToolchainDescriptor -------------------------------------------------------------------

inline Value to_json(const CXPM::ToolchainDescriptor &toolchain) {
  Map<String, Value> object;
  object["name"] = Value(toolchain.name);
  object["version"] = Value(toolchain.version);
  object["language"] = Value(toolchain.language);
  object["install_prefix"] = Value(toolchain.install_prefix);
  object["include_directory_prefix"] = Value(toolchain.include_directory_prefix);
  object["include_directories"] =
      string_collection_to_json(toolchain.include_directories);
  object["compiler_executable"] = Value(toolchain.compiler_executable);
  object["compiler_options"] =
      string_collection_to_json(toolchain.compiler_options);
  object["archiver_executable"] = Value(toolchain.archiver_executable);
  object["archiver_options"] =
      string_collection_to_json(toolchain.archiver_options);
  object["linker_executable"] = Value(toolchain.linker_executable);
  object["linker_options"] = string_collection_to_json(toolchain.linker_options);
  object["link_directory_prefix"] = Value(toolchain.link_directory_prefix);
  object["link_library_prefix"] = Value(toolchain.link_library_prefix);
  object["source_specifier_prefix"] = Value(toolchain.source_specifier_prefix);
  object["object_specifier_prefix"] = Value(toolchain.object_specifier_prefix);
  object["link_directories"] =
      string_collection_to_json(toolchain.link_directories);
  object["shared_object_prefix"] = Value(toolchain.shared_object_prefix);
  object["shared_object_suffix"] = Value(toolchain.shared_object_suffix);
  object["archive_prefix"] = Value(toolchain.archive_prefix);
  object["archive_suffix"] = Value(toolchain.archive_suffix);
  object["executable_prefix"] = Value(toolchain.executable_prefix);
  object["executable_suffix"] = Value(toolchain.executable_suffix);
  object["object_build_options"] =
      string_collection_to_json(toolchain.object_build_options);
  object["object_prefix"] = Value(toolchain.object_prefix);
  object["object_suffix"] = Value(toolchain.object_suffix);
  return Value(object);
}

inline CXPM::ToolchainDescriptor
toolchain_descriptor_from_json(const Value &value) {
  const auto &object = as_json_object(value, "a toolchain descriptor");
  CXPM::ToolchainDescriptor descriptor;

  descriptor.name = json_string_field(object, "name", descriptor.name);
  descriptor.version = json_string_field(object, "version", descriptor.version);
  descriptor.language =
      json_string_field(object, "language", descriptor.language);
  descriptor.install_prefix =
      json_string_field(object, "install_prefix", descriptor.install_prefix);
  descriptor.include_directory_prefix = json_string_field(
      object, "include_directory_prefix", descriptor.include_directory_prefix);
  descriptor.include_directories =
      json_string_array_field(object, "include_directories");
  descriptor.compiler_executable = json_string_field(
      object, "compiler_executable", descriptor.compiler_executable);
  descriptor.compiler_options =
      json_string_array_field(object, "compiler_options");
  descriptor.archiver_executable = json_string_field(
      object, "archiver_executable", descriptor.archiver_executable);
  descriptor.archiver_options =
      json_string_array_field(object, "archiver_options");
  descriptor.linker_executable = json_string_field(
      object, "linker_executable", descriptor.linker_executable);
  descriptor.linker_options = json_string_array_field(object, "linker_options");
  descriptor.link_directory_prefix = json_string_field(
      object, "link_directory_prefix", descriptor.link_directory_prefix);
  descriptor.link_library_prefix = json_string_field(
      object, "link_library_prefix", descriptor.link_library_prefix);
  descriptor.source_specifier_prefix = json_string_field(
      object, "source_specifier_prefix", descriptor.source_specifier_prefix);
  descriptor.object_specifier_prefix = json_string_field(
      object, "object_specifier_prefix", descriptor.object_specifier_prefix);
  descriptor.link_directories =
      json_string_array_field(object, "link_directories");
  descriptor.shared_object_prefix = json_string_field(
      object, "shared_object_prefix", descriptor.shared_object_prefix);
  descriptor.shared_object_suffix = json_string_field(
      object, "shared_object_suffix", descriptor.shared_object_suffix);
  descriptor.archive_prefix =
      json_string_field(object, "archive_prefix", descriptor.archive_prefix);
  descriptor.archive_suffix =
      json_string_field(object, "archive_suffix", descriptor.archive_suffix);
  descriptor.executable_prefix = json_string_field(
      object, "executable_prefix", descriptor.executable_prefix);
  descriptor.executable_suffix = json_string_field(
      object, "executable_suffix", descriptor.executable_suffix);
  descriptor.object_build_options =
      json_string_array_field(object, "object_build_options");
  descriptor.object_prefix =
      json_string_field(object, "object_prefix", descriptor.object_prefix);
  descriptor.object_suffix =
      json_string_field(object, "object_suffix", descriptor.object_suffix);

  return descriptor;
}

// --- ProjectDescriptor ----------------------------------------------------------------------

inline Value to_json(const CXPM::ProjectDescriptor &project) {
  Map<String, Value> object;

  BasicCollection<Value> targets;
  for (const auto &target : project.targets) {
    targets.push_back(to_json(target));
  }
  object["targets"] = Value(targets);

  BasicCollection<Value> toolchains;
  for (const auto &toolchain : project.toolchains) {
    toolchains.push_back(to_json(toolchain));
  }
  object["toolchains"] = Value(toolchains);

  return Value(object);
}

inline CXPM::ProjectDescriptor project_descriptor_from_json(const Value &value) {
  const auto &object = as_json_object(value, "a project descriptor");
  CXPM::ProjectDescriptor project;

  auto targets_it = object.find("targets");
  if (targets_it != object.end()) {
    if (!std::holds_alternative<BasicCollection<Value>>(targets_it->second)) {
      throw CXPM::Core::Exceptions::JsonParseException(
          "'targets' must be a JSON array");
    }
    for (const auto &element :
         std::get<BasicCollection<Value>>(targets_it->second)) {
      project.targets.push_back(target_descriptor_from_json(element));
    }
  }

  auto toolchains_it = object.find("toolchains");
  if (toolchains_it != object.end()) {
    if (!std::holds_alternative<BasicCollection<Value>>(
            toolchains_it->second)) {
      throw CXPM::Core::Exceptions::JsonParseException(
          "'toolchains' must be a JSON array");
    }
    for (const auto &element :
         std::get<BasicCollection<Value>>(toolchains_it->second)) {
      project.toolchains.push_back(toolchain_descriptor_from_json(element));
    }
  }

  return project;
}

} // namespace CXPM::Modules::Serialization
