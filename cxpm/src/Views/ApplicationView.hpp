#pragma once

#include "Controllers/ToolchainManager.hpp"
#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Tuple.hpp"
#include "Core/Exceptions/NotImplementedException.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Models/BuildOutputResult.hpp"
#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/PackageDescriptor.hpp"
#include "Models/ProjectDescriptor.hpp"
#include "Models/TargetDescriptor.hpp"
#include "Models/ToolchainDescriptor.hpp"
#include "Modules/Console/AbstractConsoleApplication.hpp"
#include "Modules/ProgramOptions/OptionDescriptorCollection.hpp"
#include "Modules/ProgramOptions/Parse.hpp"
#include "Modules/Serialization/AbstractArchiver.hpp"
#include "Modules/Serialization/JsonOutputArchiver.hpp"
#include "Utils/Unix/EnvironmentManager.hpp"
#include "Utils/Unused.hpp"
#include <filesystem>

#include <Controllers/ProjectManager.hpp>
#include <fstream>
#include <ios>
#include <semaphore>

using namespace Modules::Console;

namespace Views {
class ApplicationView final : public AbstractConsoleApplication {
public:
  ApplicationView(int argc, char *argv[])
      : AbstractConsoleApplication(argc, argv) {}

  virtual int run() override {
    auto lk = acquire_lock();

    setup();

    auto arguments = args();

    Modules::ProgramOptions::OptionsDescriptorCollection options_schema(
        "cxpm", "A package manager for C++ using C++");

    auto options = Modules::ProgramOptions::Parse(arguments);

    if (options.contains("help") || options.contains("h")) {
      print_usage();
      return 0;
    }

    if (options.contains("build") || options.contains("b")) {

      Collection<String> values;

      if (options.contains("build")) {
        values = options["build"];
      }

      if (options.contains("b")) {
        values = options["b"];
      }

      if (values.empty()) {
        throw Core::Exceptions::RuntimeException(
            "Failed to build no build directory suplied");
      }

      assert_project_directory(values.front());

      build(values.front());

      return 0;
    }

    if (options.contains("install") || options.contains("i")) {

      Collection<String> values;
      String prefix = "/usr/local";

      if (options.contains("install")) {
        values = options["install"];
      }

      if (options.contains("i")) {
        values = options["i"];
      }

      if (options.contains("prefix")) {
        prefix = options["prefix"].front();
      }

      auto [status, project] = install_project(values.front(), prefix);

      switch (status) {
      case InstallProjectResultStatus::Failure:
      case InstallProjectResultStatus::Success:
        break;
      }

      return 0;
    }

    print_usage();

    return 1;
  }

  void print_usage() {
    std::osyncstream(std::cout) << R"(
cxpm: A simple package manager written in C++ for C++
Usage:
    cxpm [option] [arguments]
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

    Controllers::ProjectManager::initialize();
    return 0;
  }

private:
  Controllers::ProjectManager::BuildProjectOutputResult
  build(const String &directory) {
    return Controllers::ProjectManager::build_project(directory);
  }

  Controllers::ProjectManager::InstallTargetOutputResult
  install_target(TargetDescriptor &target,
                 const String &prefix_override = "/usr/local") {

    auto [result, build, toolchain] =
        Controllers::ProjectManager::build_target(target, prefix_override);

    switch (result) {
    case Controllers::ProjectManager::BuildTargetOutputResultStatus::Success:
      break;
    case Controllers::ProjectManager::BuildTargetOutputResultStatus::Failure:
      return {
          Controllers::ProjectManager::InstallTargetOutputResultStatus::Failure,
          build, toolchain};
      break;
    }

    return Controllers::ProjectManager::install_target(target, toolchain,
                                                       prefix_override);
  }

  enum class InstallProjectResultStatus { Success, Failure };

  using InstallProjectResult =
      std::tuple<InstallProjectResultStatus, ProjectDescriptor>;

  InstallProjectResult install_project(const String &directory,
                                       const String &prefix = "/usr/local") {

    auto [result, project, toolchain] = build(directory);

    for (auto target : project.targets) {
      auto [result, target_result, target_toolchain] =
          install_target(target, prefix);

      if (result != Controllers::ProjectManager::
                        InstallTargetOutputResultStatus::Success) {
        return {InstallProjectResultStatus::Failure, project};
      }
    }

    return {InstallProjectResultStatus::Success, project};
  }

  void uninstall(const String &directory) {
    Utils::Unused{directory};
    throw Core::Exceptions::NotImplementedException{};
  }

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
};
} // namespace Views