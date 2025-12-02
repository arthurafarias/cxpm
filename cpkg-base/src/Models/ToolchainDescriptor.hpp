#pragma once

#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>

using namespace Core::Containers;
namespace Models {
struct ToolchainDescriptor {
  String name;
  String version;
  String language;

  String install_prefix;

  String include_directory_prefix = "-I";
  Collection<String> include_directories;

  String compiler_executable;
  Collection<String> compiler_options;

  String archiver_executable;
  Collection<String> archiver_options;

  String linker_executable;
  Collection<String> linker_options;

  String link_directory_prefix = "-L";
  String link_library_prefix = "-l";
  String source_specifier_prefix = "-c";
  String object_specifier_prefix = "-o";
  Collection<String> link_directories;
};
} // namespace Models