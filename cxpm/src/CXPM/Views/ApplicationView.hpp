#pragma once

#include "CXPM/Controllers/ToolchainManager.hpp"
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Exceptions/NotImplementedException.hpp>
#include <CXPM/Core/Exceptions/RuntimeException.hpp>
#include <CXPM/Models/ProjectDescriptor.hpp>
#include <CXPM/Models/TargetDescriptor.hpp>
#include <CXPM/Modules/Console/AbstractConsoleApplication.hpp>
#include <CXPM/Modules/Logging/LoggerManager.hpp>
#include <CXPM/Modules/ProgramOptions/OptionDescriptorCollection.hpp>
#include <CXPM/Modules/ProgramOptions/Parse.hpp>
#include <CXPM/Utils/Unused.hpp>
#include <CXPM/Views/Resources/ApplicationViewHelpText.hpp>
#include <filesystem>

#include <CXPM/Controllers/ProjectManager.hpp>
#include <ranges>

using namespace CXPM::Modules::Console;

namespace CXPM::Views {
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
    std::osyncstream(std::cout) << Resources::ApplicationViewHelpText;
  }

protected:
  int setup() override {

    Modules::Logging::LoggerManager::stream_set(
        Modules::Logging::LoggerManager::stream_cout());
    Modules::Logging::LoggerManager::level_set(
        Modules::Logging::LoggerManager::Level::Max);

    Controllers::ProjectManager::initialize();
    Controllers::ToolchainManager::initialize();
    return 0;
  }

private:
  void generate(const String &directory) {

    Collection<CompileCommandDescriptor> project_compile_commands;

    auto build_path = std::filesystem::path(directory.c_str());

    if (!directory.empty()) {
      build_path = directory.c_str();
    }

    auto [result, compile_commands] =
        Controllers::ProjectManager::manifest_generate(build_path.string());

    project_compile_commands.append_range(compile_commands);

    if (result != 0) {

      throw Core::Exceptions::RuntimeException(
          "Failed to generate manifest {} using {}",
          build_path.string() + "/project-manifest.so",
          build_path.string() + "/package.json");
    }

    auto project_manifest =
        Controllers::ProjectManager::manifest_load_from_path(
            std::filesystem::path(build_path.string())
                .append("libproject-manifest.so")
                .string());

    project_manifest.compile_comands = compile_commands;
    project_manifest.build_path = directory;

    for (auto target : project_manifest.targets) {

      if (target.type == "reference") {

        auto path = std::filesystem::path(target.url.c_str());

        if (path.is_relative()) {
          path = std::filesystem::path(directory.c_str()).append(path.c_str());
        }

        generate(path.c_str());
      }
    }

    auto non_reference_targets =
        project_manifest.targets |
        std::views::filter([](auto &&v) { return v.type != "reference"; }) |
        std::ranges::to<Collection<TargetDescriptor>>();

    for (auto& target : non_reference_targets) {
      target.build_path = directory;
    }

    Controllers::ProjectManager::targets_append(non_reference_targets);
  }

  Controllers::ProjectManager::BuildProjectOutputResult
  build(const String &directory) {

    generate(directory);

    return Controllers::ProjectManager::build_all();
  }

  Controllers::ProjectManager::InstallTargetOutputResult
  install_target(TargetDescriptor &target,
                 const String &prefix_override = "/usr/local") {

    auto [result, build, toolchain] =
        Controllers::ProjectManager::target_build(target, prefix_override);

    switch (result) {
    case Controllers::ProjectManager::BuildTargetOutputResultStatus::Success:
      break;
    case Controllers::ProjectManager::BuildTargetOutputResultStatus::Failure:
      return {
          Controllers::ProjectManager::InstallTargetOutputResultStatus::Failure,
          build, toolchain};
      break;
    }

    return Controllers::ProjectManager::target_install(target, toolchain,
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
} // namespace CXPM::Views