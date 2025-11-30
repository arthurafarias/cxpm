#include "Models/ToolchainDescriptor.hpp"
#include <Controllers/ProjectManager.hpp>
#include <Controllers/ToolchainManager.hpp>
#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Exceptions/RuntimeException.hpp>
#include <Core/Logging/LoggerManager.hpp>
#include <Utils/Unix/EnvironmentManager.hpp>

#include <algorithm>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include <dlfcn.h>

int print_usage() {
  std::cout << R"(
Usage: cpkg-build [options]
    --help: print usage
    --build: build project
    --generate: generate project
)";
  return 0;
}

int main(int argc, char *argv[]) {

  Core::Containers::Collection<Core::Containers::String> args(argv,
                                                              argv + argc);

  auto build_path = std::filesystem::current_path();

  auto extra_modules_path =
      Utils::Unix::EnvironmentManager::get("CPKG_BUILD_EXTRA_MODULES_PATH");

  Core::Logging::LoggerManager::stream_set(
      Core::Logging::LoggerManager::stream_cout());
  Core::Logging::LoggerManager::level_set(
      Core::Logging::LoggerManager::Level::Max);

  if (args.size() == 2 && args.back() == "") {
    args[1] = "--help";
  }

  if (std::find_if(args.begin(), args.end(),
                   [](auto str) { return str == "--path"; }) != args.end()) {
  }

  if (std::find_if(args.begin(), args.end(), [](auto str) {
        return str == "--generate";
      }) != args.end()) {
    return Controllers::ProjectManager::build_manifest(build_path.string(),
                                                       extra_modules_path);
  }

  if (std::find_if(args.begin(), args.end(),
                   [](auto str) { return str == "--clean"; }) != args.end()) {

    if (Controllers::ProjectManager::build_manifest(build_path.string(),
                                                    extra_modules_path) != 0) {
      return -1;
    }

    if (Controllers::ProjectManager::clean(build_path.string(),
                                           extra_modules_path) != 0) {
      return -1;
    }

    return 0;
  }

  if (args[1] == "--build") {

    if (Controllers::ProjectManager::build_manifest(build_path.string(),
                                                    extra_modules_path) != 0) {
      return -1;
    }

    auto project_manifest = Controllers::ProjectManager::load_from_manifest(
        std::filesystem::path(build_path.string())
            .append("project-manifest.so")
            .string());

    for (auto toolchain : project_manifest.toolchains) {
      try {

        if (Controllers::ToolchainManager::valid(toolchain)) {
          Controllers::ToolchainManager::add(toolchain);
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error(
            "Couldn't append toolchain specified in project {}", ex.what());
      }
    }

    for (auto target : project_manifest.targets) {
      try {

        auto toolchain = Controllers::ToolchainManager::autoselect(target);

        if (target.toolchain != "") {
          toolchain = Controllers::ToolchainManager::by_name(target.toolchain);
        }

        if (toolchain.build(target) != 0) {
          throw Core::Exceptions::RuntimeException(
              "Couldn't build project {} with language {} and toolchain {}",
              target.name, target.language, toolchain.name);
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error("Couldn't build the project: {}",
                                            ex.what());
      }
    }
  }

  if (args[1] == "--install") {
    if (Controllers::ProjectManager::build_manifest(build_path.string(),
                                                    extra_modules_path) != 0) {
      return -1;
    }

    auto project_manifest = Controllers::ProjectManager::load_from_manifest(
        std::filesystem::path(build_path.string())
            .append("project-manifest.so")
            .string());

    Controllers::ToolchainManager::current().install(project_manifest);

    return 0;
  }

  if (args[1] == "--help") {
    return print_usage();
  }

  return 1;
}