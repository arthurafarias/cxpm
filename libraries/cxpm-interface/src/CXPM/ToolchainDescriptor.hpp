#pragma once

#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;
namespace Models {
struct ToolchainDescriptor {
  String name;
  String version;
  String language;

  String install_prefix;

  String include_directory_prefix = "-I";
  BasicCollection<String> include_directories;

  String compiler_executable;
  BasicCollection<String> compiler_options;

  String archiver_executable;
  BasicCollection<String> archiver_options;

  String linker_executable;
  BasicCollection<String> linker_options;

  String link_directory_prefix = "-L";
  String link_library_prefix = "-l";
  String source_specifier_prefix = "-c";
  String object_specifier_prefix = "-o";
  BasicCollection<String> link_directories;

  String shared_object_prefix = "lib";
  String shared_object_suffix = ".so";

  String archive_prefix = "lib";
  String archive_suffix = ".a";

  String executable_prefix = "";
  String executable_suffix = "";

  String object_prefix = "";
  String object_suffix = ".o";

};
} // namespace Models