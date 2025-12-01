#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Modules/Console/AbstractConsoleApplication.hpp"
#include "Modules/ProgramOptions/OptionDescriptorCollection.hpp"
#include "Modules/ProgramOptions/Parse.hpp"
#include "Modules/Serialization/JsonOutputArchiver.hpp"
#include "Utils/Unix/EnvironmentManager.hpp"
#include <filesystem>

#include <Controllers/ProjectManager.hpp>

using namespace Modules::Console;

#ifndef CPKG_BUILD_INSTALL_PREFIX
#define CPKG_BUILD_INSTALL_PREFIX "/usr"
#endif

namespace Views {
class ApplicationView final : public AbstractConsoleApplication {
public:
  ApplicationView(int argc, char *argv[])
      : AbstractConsoleApplication(argc, argv) {}

  virtual int run() override {
    auto lk = acquire_lock();

    setup();

    auto arguments = args();

    Modules::ProgramOptions::OptionsDescriptorCollection options_schema{
        .name = "cpkg-build",
        .description = "A package manager for C++ using C++"};

    auto options = Modules::ProgramOptions::Parse(arguments);

    for (auto [key, values] : options) {

      if (key == "help" || key == "h") {
        print_usage();
        return 0;
      }

      if (key == "build" || key == "b") {

        if (values.empty()) {
          throw Core::Exceptions::RuntimeException(
              "Failed to build no build directory suplied");
        }

        assert_project_directory(values.front());

        build(values.front());

        return 0;
      }

      if (key == "install" || key == "i") {

        if (values.empty()) {
          throw Core::Exceptions::RuntimeException(
              "Failed to build no build directory suplied");
        }

        assert_project_directory(values.front());

        install(values.front());

        return 0;
      }

      if (key == "uninstall" || key == "u") {

        if (values.empty()) {
          throw Core::Exceptions::RuntimeException(
              "Failed to build no build directory suplied");
        }

        uninstall(values.front());

        return 0;
      }
    }

    print_usage();

    return 1;
  }

  void print_usage() {
    std::osyncstream(std::cout) << R"(
cpkg-build: A simple package manager written in C++ for C++
Usage:
    cpkg-build [option] [arguments]
Options:

    -b|--build <directory>: Build project on directory containing package.cpp
    -i|--install <directory>: Install project on directory containing package.cpp
    -u|--uninstall <directory>: Uninstall project on directory containing package.cpp

    )";
  }

protected:
  int setup() override {

    Core::Logging::LoggerManager::stream_set(
        Core::Logging::LoggerManager::stream_cout());
    Core::Logging::LoggerManager::level_set(
        Core::Logging::LoggerManager::Level::Max);
    return 0;
  }

private:
  Collection<String> modules_search_paths;

  void build(const String &directory) {

    auto build_path = std::filesystem::current_path();

    if (!directory.empty()) {
      build_path = directory.c_str();
    }

    modules_search_paths =
        Utils::Unix::EnvironmentManager::get("CPKG_BUILD_EXTRA_MODULES_PATH");

    extra_module_path_add("/usr/share/cpkg/toolchains");
    extra_module_path_add("/usr/local/share/cpkg/toolchains");

    extra_module_path_add("/usr/lib/cpkg/toolchains");
    extra_module_path_add("/usr/local/lib/cpkg/toolchains");

    extra_module_path_add(CPKG_BUILD_INSTALL_PREFIX "/lib/cpkg/toolchains");
    extra_module_path_add(CPKG_BUILD_INSTALL_PREFIX "/share/cpkg/toolchains");

    if (Controllers::ProjectManager::build_manifest(
            build_path.string(), modules_search_paths) != 0) {

      throw Core::Exceptions::RuntimeException(
          "Failed to build manifest {} using {}",
          build_path.string() + "/project-manifest.so",
          build_path.string() + "/package.json");
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

        auto [result, commands] = toolchain.build(target);

        auto stream = std::ofstream("compile_commands.json");
        Modules::Serialization::JsonOutputArchiver output(stream);
        output % commands;

        if (result != 0) {
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

  void install(const String &directory) {}
  void uninstall(const String &directory) {}

  void assert_project_directory(const String &directory) {
    if (!std::filesystem::exists(directory.c_str())) {
      throw Core::Exceptions::RuntimeException(
          "Failed to build directory doesn't exists");
    }

    if (!std::filesystem::is_directory(directory.c_str())) {
      throw Core::Exceptions::RuntimeException(
          "Failed to build folder isn't a directory");
    }

    auto project_file = directory + "/package.cpp";

    if (!std::filesystem::exists(project_file) ||
        !std::filesystem::is_regular_file(project_file)) {
      throw Core::Exceptions::RuntimeException(
          "Failed to find package.cpp in {}", directory);
    }
  }

  void extra_module_path_add(const char *try_path) {
    if (std::filesystem::exists(try_path) &&
        std::filesystem::is_directory(try_path)) {
      using directory_iterator = std::filesystem::directory_iterator;
      for (auto dir : directory_iterator(try_path)) {
        if (dir.is_directory()) {
          modules_search_paths.push_back(dir.path().string());
        }
      }
    }
  }
};
} // namespace Views