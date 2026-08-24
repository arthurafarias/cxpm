#pragma once

// Backs `cxpm --generate` (see docs/SRS-generate.md): produces starter package.cpp/toolchain.cpp
// source text and package.json/toolchain.json manifests. The JSON variants are not hand-written
// string templates -- they are built from a real ProjectDescriptor/ToolchainDescriptor run
// through the same to_json()/write_json_pretty() serializers everything else in this codebase
// uses (JsonManifest.hpp, JsonValueWriter.hpp), so a generated package.json is guaranteed
// parseable by ProjectManager::load_project() and demonstrates the serializers on real data.

#include "CXPM/Modules/Serialization/JsonManifest.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/Target.hpp"
#include "CXPM/Toolchain.hpp"

#include <format>

namespace CXPM::Modules::Generators {

using CXPM::Core::Containers::String;

inline String generate_package_cpp_template(const String &target_name) {
  return String(std::format(
      R"(#include <CXPM/Target.hpp>
#include <CXPM/Project.hpp>

using namespace CXPM;

auto example = Target()
                   .name_set("{}")
                   .version_set("1.0.0")
                   .type_set("executable")
                   .sources_append({{"src/main.cpp"}})
                   .options_append({{"-fPIE", "-fstack-protector-all"}})
                   .link_libraries_append({{"m"}})
                   .include_directories_append({{"src"}})
                   .create();

auto project = Project().add(example).create();
)",
      target_name.c_str()));
}

inline String generate_package_json_template(const String &target_name) {
  auto target = CXPM::Target()
                    .name_set(target_name)
                    .version_set("1.0.0")
                    .type_set("executable")
                    .sources_append({"src/main.cpp"})
                    .options_append({"-fPIE", "-fstack-protector-all"})
                    .link_libraries_append({"m"})
                    .include_directories_append({"src"})
                    .create();

  CXPM::ProjectDescriptor project;
  project.targets.push_back(target);

  using namespace CXPM::Modules::Serialization;
  return write_json_pretty(to_json(project)) + "\n";
}

inline String generate_toolchain_cpp_template(const String &toolchain_name) {
  return String(std::format(
      R"(#include <CXPM/Toolchain.hpp>

CXPM::ToolchainDescriptor toolchain = CXPM::Toolchain()
                     .name_set("{}")
                     .version_set("generic")
                     .include_directory_prefix_set("-I")
                     .link_directory_prefix_set("-L")
                     .link_library_prefix_set("-l")
                     .compiler_executable_set("/usr/bin/c++")
                     .linker_executable_set("/usr/bin/c++")
                     .archiver_executable_set("/usr/bin/ar")
                     .archiver_options_set({{"rcs"}})
                     .language_set("c++")
                     .compiler_options_set({{"-pthread"}})
                     .object_prefix_set("")
                     .object_suffix_set(".o")
                     .shared_object_prefix_set("lib")
                     .shared_object_suffix_set(".so")
                     .archive_prefix_set("lib")
                     .archive_suffix_set(".a")
                     .executable_prefix_set("")
                     .executable_suffix_set("");
)",
      toolchain_name.c_str()));
}

inline String generate_toolchain_json_template(const String &toolchain_name) {
  auto toolchain = CXPM::Toolchain()
                        .name_set(toolchain_name)
                        .version_set("generic")
                        .language_set("c++")
                        .compiler_executable_set("/usr/bin/c++")
                        .linker_executable_set("/usr/bin/c++")
                        .archiver_executable_set("/usr/bin/ar");

  using namespace CXPM::Modules::Serialization;
  return write_json_pretty(to_json(
             static_cast<const CXPM::ToolchainDescriptor &>(toolchain))) +
         "\n";
}

} // namespace CXPM::Modules::Generators
