#pragma once

#include "CXPM/Core/Exceptions/RuntimeException.hpp"
#include "CXPM/Models/TargetDescriptor.hpp"
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Models/ToolchainDescriptor.hpp>
#include <CXPM/Utils/Unix/ShellManager.hpp>

#include <CXPM/Models/PackageDescriptor.hpp>
#include <CXPM/Utils/Macros/StaticClass.hpp>

using namespace CXPM::Core::Containers;
using namespace CXPM::Models;

namespace CXPM::Controllers {

class PackageConfigManager final {

  StaticClass(PackageConfigManager);

public:
  static inline PackageDescriptor find_package(const String &name) {

    PackageDescriptor package;

    auto cmdline = std::format("pkg-config --cflags --libs {}", name);

    auto [code, out, err] = Utils::Unix::ShellManager::exec(cmdline);

    if (code != 0) {
      throw Core::Exceptions::RuntimeException("{}", err);
    }

    auto splitted = String::split(out, " ");

    for (auto el : splitted) {

      if (el.starts_with("-I")) {
        package.include_directories.insert(el.substr(2, el.size() - 2));
        continue;
      }

      if (el.starts_with("-L")) {
        package.link_directories.insert(el.substr(2, el.size() - 2));
        continue;
      }

      if (el.starts_with("-l")) {
        package.link_libraries.insert(el.substr(2, el.size() - 2));
        continue;
      }

      package.options.insert(el);
    }

    return package;
  }

  static inline void solve_dependencies(TargetDescriptor &target) {

    auto pkg_config_dependencies =
        target.link_libraries |
        std::views::transform(
            [](const String &library) -> std::optional<PackageDescriptor> {
              try {
                PackageDescriptor result =
                    Controllers::PackageConfigManager::find_package(library);
                return result;
              } catch (...) {
                return std::nullopt;
              }
            }) |
        std::views::filter([](auto &&result) { return result.has_value(); }) |
        std::views::transform([](auto &&result) { return result.value(); });

    for (auto descriptor : pkg_config_dependencies) {
      target.include_directories.insert_range(descriptor.include_directories);
      target.link_directories.insert_range(descriptor.link_directories);
      target.link_libraries.insert_range(descriptor.link_libraries);
      target.options.insert_range(descriptor.options);
    }
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

} // namespace CXPM::Controllers