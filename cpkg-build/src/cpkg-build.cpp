#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Models/BasicToolchain.hpp"
#include <Controllers/Project.hpp>
#include <Controllers/Toolchain.hpp>
#include <Models/BasicTarget.hpp>
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

  std::deque<std::string> args(argv, argv + argc);

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
    return Controllers::Project::build_manifest(build_path, extra_modules_path);
  }

  if (std::find_if(args.begin(), args.end(),
                   [](auto str) { return str == "--clean"; }) != args.end()) {

    if (Controllers::Project::build_manifest(build_path, extra_modules_path) !=
        0) {
      return -1;
    }

    if (Controllers::Project::clean(build_path, extra_modules_path) != 0) {
      return -1;
    }

    return 0;
  }

  if (args[1] == "--build") {

    if (Controllers::Project::build_manifest(build_path, extra_modules_path) !=
        0) {
      return -1;
    }

    auto project_manifest = Controllers::Project::load_from_manifest(
        std::filesystem::path(build_path).append("project-manifest.so"));

    for (auto target : project_manifest->targets) {
      try {

        if (!std::holds_alternative<Models::ToolchainDescriptor>(target)) {
          // ignore other than toolchains
          continue;
        }

        auto toolchain = std::make_shared<Controllers::ToolchainManager>(
            std::get<Models::ToolchainDescriptor>(target));

        if (Controllers::ToolchainManager::valid(toolchain)) {
          Controllers::ToolchainManager::add(toolchain);
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error(
            "Couldn't append toolchain specified in project {}", ex.what());
      }
    }

    for (auto target_variant : project_manifest->targets) {
      try {

        if (!std::holds_alternative<Models::BasicTarget>(target_variant)) {
          // ignore other than toolchains
          continue;
        }

        auto target = std::get<Models::BasicTarget>(target_variant);

        auto toolchain = Controllers::ToolchainManager::autoselect(target);

        if (target.toolchain.as<String>() != "") {
          Controllers::ToolchainManager::by_name(
              std::get<String>(target.toolchain));
          throw Core::Exceptions::RuntimeException(
              "Failed to find toolchain {} for project {}",
              std::get<String>(target.toolchain),
              std::get<String>(target.name));
        }

        if (toolchain == nullptr) {
          throw Core::Exceptions::RuntimeException(
              "Couldn't select a valid toolchain for project {} with language "
              "{}",
              target.name.as<String>(), target.language.as<String>());
        }

        if (toolchain->build(target) != 0) {
          throw Core::Exceptions::RuntimeException(
              "Couldn't build project {} with language {} and toolchain {}",
              target.name.as<String>(), target.language.as<String>(),
              toolchain->name.as<String>());
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error("Couldn't build the project: {}",
                                            ex.what());
      }
    }

    // return Controllers::Toolchain::autoselect(project_manifest)
    //     ->build(project_manifest);
  }

  if (args[1] == "--install") {
    if (Controllers::Project::build_manifest(build_path, extra_modules_path) !=
        0) {
      return -1;
    }

    auto project_manifest = Controllers::Project::load_from_manifest(
        std::filesystem::path(build_path).append("project-manifest.so"));

    Controllers::ToolchainManager::current()->install(project_manifest);

    return 0;
  }

  if (args[1] == "--help") {
    return print_usage();
  }

  return 1;
}