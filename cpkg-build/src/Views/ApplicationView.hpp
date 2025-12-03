#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Tuple.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/PackageDescriptor.hpp"
#include "Models/ProjectDescriptor.hpp"
#include "Models/TargetDescriptor.hpp"
#include "Modules/Console/AbstractConsoleApplication.hpp"
#include "Modules/ProgramOptions/OptionDescriptorCollection.hpp"
#include "Modules/ProgramOptions/Parse.hpp"
#include "Modules/Serialization/AbstractArchiver.hpp"
#include "Modules/Serialization/JsonOutputArchiver.hpp"
#include "Utils/Unix/EnvironmentManager.hpp"
#include <Modules/Templating/MustacheLite.hpp>
#include <filesystem>

#include <Controllers/ProjectManager.hpp>
#include <fstream>
#include <ios>
#include <semaphore>

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

    Modules::ProgramOptions::OptionsDescriptorCollection options_schema(
        "cpkg-build", "A package manager for C++ using C++");

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

  enum class BuildResultStatus { Success, Failure };

  using BuildResult = std::tuple<BuildResultStatus, ProjectDescriptor>;

  BuildResult build(const String &directory) {

    Collection<CompileCommandDescriptor> commands;

    auto build_path = std::filesystem::path(directory.c_str());

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

    auto [build_manifest_result, build_manifest_comands] =
        Controllers::ProjectManager::build_manifest(build_path.string(),
                                                    modules_search_paths);

    commands.append_range(build_manifest_comands);

    if (build_manifest_result != 0) {

      throw Core::Exceptions::RuntimeException(
          "Failed to build manifest {} using {}",
          build_path.string() + "/project-manifest.so",
          build_path.string() + "/package.json");
    }

    auto project_manifest = Controllers::ProjectManager::load_from_manifest(
        std::filesystem::path(build_path.string())
            .append("libproject-manifest.so")
            .string());

    project_manifest.compile_comands = build_manifest_comands;

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

        auto [build_result, build_commands] = toolchain.build(target);

        target.compile_commands = build_commands;

        commands.append_range(target.compile_commands);

        if (build_result != 0) {
          throw Core::Exceptions::RuntimeException(
              "Couldn't build project {} with language {} and toolchain {}",
              target.name, target.language, toolchain.name);
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error("Couldn't build the project: {}",
                                            ex.what());
      }
    }

    auto stream = std::ofstream("compile_commands.json");
    Modules::Serialization::JsonOutputArchiver output(stream);
    output % commands;

    return {BuildResultStatus::Success, project_manifest};
  }

  enum class InstallTargetResultStatus {
    Success,
    Failure,
  };

  using InstallTargetResult =
      std::tuple<InstallTargetResultStatus, TargetDescriptor>;

  InstallTargetResult install_target(TargetDescriptor &target,
                                     const String& prefix = "/usr/local") {

    // include directory should be in project directory
    using directory_iterator = std::filesystem::directory_iterator;

    target.install_prefix = prefix;

    // install include directories
    for (auto directory : target.include_directories) {

      auto header_install_path = std::filesystem::path()
                                     .append(prefix.c_str())
                                     .append("include")
                                     .append(target.name.c_str());

      auto binnaries_install_path =
          std::filesystem::path().append(prefix.c_str()).append("bin");

      auto archive_install_path =
          std::filesystem::path().append(prefix.c_str()).append("lib");

      auto library_install_path =
          std::filesystem::path().append(prefix.c_str()).append("lib");

      auto pc_install_path = std::filesystem::path()
                                 .append(prefix.c_str())
                                 .append("lib")
                                 .append("pkgconfig");

      if (!std::filesystem::is_directory(header_install_path)) {
        std::filesystem::create_directories(header_install_path);
      }

      if (!std::filesystem::is_directory(binnaries_install_path)) {
        std::filesystem::create_directories(binnaries_install_path);
      }

      if (!std::filesystem::is_directory(library_install_path)) {
        std::filesystem::create_directories(library_install_path);
      }

      if (!std::filesystem::is_directory(archive_install_path)) {
        std::filesystem::create_directories(archive_install_path);
      }

      if (!std::filesystem::is_directory(pc_install_path)) {
        std::filesystem::create_directories(pc_install_path);
      }

      for (auto file : directory_iterator(directory.c_str())) {

        if (!(file.path().extension() == ".h") &&
            !(file.path().extension() == ".hpp") &&
            !(file.path().extension() == ".hxx") &&
            !(file.path().extension() == ".hh")) {
          continue;
        }

        std::filesystem::copy(
            file, header_install_path,
            std::filesystem::copy_options::recursive |
                std::filesystem::copy_options::overwrite_existing);
      }

      if (target.type == "executable") {
        std::filesystem::copy(
            target.name.c_str(),
            std::filesystem::path(binnaries_install_path.c_str()),
            std::filesystem::copy_options::overwrite_existing);
      }

      if (target.type == "shared-library") {
        std::filesystem::copy(
            "lib" + target.name + ".so",
            std::filesystem::path(library_install_path.c_str()),
            std::filesystem::copy_options::overwrite_existing);
      }

      if (target.type == "static-library") {
        std::filesystem::copy(
            "lib" + target.name + ".a",
            std::filesystem::path(archive_install_path.c_str()),
            std::filesystem::copy_options::overwrite_existing);
      }

      if (target.type == "object-library") {
        std::filesystem::copy(
            target.name + ".o",
            std::filesystem::path(library_install_path.c_str()),
            std::filesystem::copy_options::overwrite_existing);
      }

      // install pc file
      {
        using namespace Models;
        using namespace Modules::Templating;
        String view = {R"(
Name: {{name}}
Description: {{description}}
Version: {{version}}
URL: {{url}}
Cflags: -I{{install_prefix}}/include/{{name}}
Libs: -l{{name}}
          )"};
        auto renderer = TemplateEngine(view);
        renderer % target;

        auto rendered = renderer.render();

        auto pc_file_stream =
            std::ofstream(pc_install_path.append(target.name + ".pc"),
                          std::ios_base::out | std::ios_base::trunc);
        auto syncstream = std::osyncstream(pc_file_stream);
        syncstream << rendered;
      }
    }

    return {InstallTargetResultStatus::Success, target};
  }

  enum class InstallProjectResultStatus { Success, Failure };

  using InstallProjectResult =
      std::tuple<InstallProjectResultStatus, ProjectDescriptor>;

  InstallProjectResult install_project(const String &directory,
                                       const String &prefix = "/usr/local") {
    auto [res, project] = build(directory);

    for (auto target : project.targets) {
      auto [result, target_result] = install_target(target, prefix);
    }

    return {InstallProjectResultStatus::Success, project};
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