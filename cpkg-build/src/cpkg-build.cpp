#include <Controllers/Project.hpp>
#include <Controllers/Toolchain.hpp>
#include <Models/BasicTarget.hpp>
#include <Utils/Unix/EnvironmentManager.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
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

    return Controllers::Toolchain::current()->build(project_manifest);
  }

  if (args[1] == "--install") {
    if (Controllers::Project::build_manifest(build_path, extra_modules_path) !=
        0) {
      return -1;
    }

    auto project_manifest = Controllers::Project::load_from_manifest(
        std::filesystem::path(build_path).append("project-manifest.so"));

    Controllers::Toolchain::current()->install(project_manifest);

    return 0;
  }

  if (args[1] == "--help") {
    return print_usage();
  }

  return 1;
}