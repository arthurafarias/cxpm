#pragma once

#include "CXPM/Core/Containers/Collection.hpp"
#include "CXPM/Core/Exceptions/RuntimeException.hpp"
#include "CXPM/Core/Logging/LoggerManager.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/Modules/Console/AbstractConsoleApplication.hpp"
#include "CXPM/Modules/ProgramOptions/OptionDescriptorCollection.hpp"
#include "CXPM/Modules/ProgramOptions/Parse.hpp"
#include <filesystem>

#include <CXPM/ProjectManager.hpp>

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

      build(std::filesystem::absolute(values.front().c_str()));

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

      auto [status, project] = install_project(std::filesystem::absolute(values.front().c_str()), prefix);

      switch (status) {
      case Status::Failure:
      case Status::Success:
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
    case Status::Success:
      break;
    case Status::Failure:
      return {
          Status::Failure,
          build, toolchain};
      break;
    }

    return Controllers::ProjectManager::install_target(target, toolchain, prefix_override);
  }

  

  using InstallProjectResult =
      std::tuple<Status, ProjectDescriptor>;

  InstallProjectResult install_project(const String &directory,
                                       const String &prefix = "/usr/local") {

    auto [result, project, toolchain] = build(directory);

    for (auto target : project.targets) {
      auto [result, target_result, target_toolchain] =
          install_target(target, prefix);

      if (result != Status::Success) {
        return {Status::Failure, project};
      }
    }

    return {Status::Success, project};
  }

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
};
} // namespace Views