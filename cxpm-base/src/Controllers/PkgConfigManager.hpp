#pragma once

#include "Models/ToolchainDescriptor.hpp"
#include "Utils/Unix/ShellManager.hpp"
#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>

#include <Models/PackageDescriptor.hpp>
#include <Utils/Macros/StaticClass.hpp>

using namespace Core::Containers;
using namespace Models;

namespace Controllers {

class PackageConfigManager final {

  StaticClass(PackageConfigManager)

public:
  static inline PackageDescriptor find_package(const String &name) {

    PackageDescriptor package;

    auto cmdline = std::format("pkg-config --cflags --libs {}", name.c_str());
    auto [code, out, err] = Utils::Unix::ShellManager::exec(cmdline);

    auto splitted = String::split(out, " ");

    for (auto el : splitted) {

      if (el.starts_with("-I")) {
        package.include_directories.push_back(el.substr(2, el.size() - 2));
        continue;
      }

      if (el.starts_with("-L")) {
        package.link_directories.push_back(el.substr(2, el.size() - 2));
        continue;
      }

      if (el.starts_with("-l")) {
        package.link_libraries.push_back(el.substr(2, el.size() - 2));
        continue;
      }

      package.options.push_back(el);
    }

    return package;
  }

  static inline String
  emit_configuration(const PackageDescriptor &package,
                     const ToolchainDescriptor &toolchain) {
    String configuration = BasePackageConfiguration;

    auto prefix_needle = String("{{prefix}}");
    auto name_needle = String("{{name}}");
    auto version_needle = String("{{version}}");
    auto description_needle = String("{{description}}");

    configuration.replace(configuration.find(name_needle), name_needle.size(),
                          package.name);
    configuration.replace(configuration.find(prefix_needle),
                          prefix_needle.size(), toolchain.install_prefix);
    configuration.replace(configuration.find(version_needle),
                          version_needle.size(), package.name);
    configuration.replace(configuration.find(description_needle),
                          description_needle.size(), toolchain.install_prefix);

    return configuration;
  }

  static inline const String BasePackageConfiguration = R"(
prefix={{prefix}}
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: lib{{name}}
Description: {{description}}
Version: {{version}}
Cflags: -I${includedir}/lib{{name}}
Libs: -L${libdir} -l{{name}}
  )";
};

} // namespace Controllers