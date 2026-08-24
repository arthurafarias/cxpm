#pragma once

#include "CXPM/Core/Exceptions/RuntimeException.hpp"
#include "CXPM/Core/Logging/LoggerManager.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/Modules/Console/AbstractConsoleApplication.hpp"
#include "CXPM/Modules/Generators/ManifestGenerator.hpp"
#include "CXPM/Modules/ProgramOptions/OptionDescriptorCollection.hpp"
#include "CXPM/Modules/ProgramOptions/Parse.hpp"
#include "CXPM/Utils/Unused.hpp"
#include <filesystem>
#include <fstream>

#include <CXPM/ProjectManager.hpp>

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

    CXPM::Modules::ProgramOptions::OptionsDescriptorCollection options_schema(
        "cxpm", "A package manager for C++ using C++");

    auto options = CXPM::Modules::ProgramOptions::Parse(arguments);

    if (options.contains("help") || options.contains("h")) {
      print_usage();
      return 0;
    }

    if (options.contains("build") || options.contains("b")) {

      BasicCollection<String> values;

      if (options.contains("build")) {
        values = options["build"];
      }

      if (options.contains("b")) {
        values = options["b"];
      }

      if (values.empty()) {
        throw CXPM::Core::Exceptions::RuntimeException(
            "Failed to build no build directory suplied");
      }

      assert_project_directory(values.front());

      build(std::filesystem::absolute(values.front().c_str()));

      return 0;
    }

    if (options.contains("install") || options.contains("i")) {

      BasicCollection<String> values;
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

      if (values.empty()) {
        throw CXPM::Core::Exceptions::RuntimeException(
            "Failed to install no install directory suplied");
      }

      assert_project_directory(values.front());

      auto [status, project] = install_project(std::filesystem::absolute(values.front().c_str()), prefix);

      switch (status) {
      case Status::Failure:
        return 1;
      case Status::Success:
        break;
      }

      return 0;
    }

    if (options.contains("generate") || options.contains("g")) {

      BasicCollection<String> values;

      if (options.contains("generate")) {
        values = options["generate"];
      }

      if (options.contains("g")) {
        values = options["g"];
      }

      if (values.empty()) {
        throw CXPM::Core::Exceptions::RuntimeException(
            "Failed to generate: no kind supplied (expected one of "
            "package-cpp, package-json, toolchain-cpp, toolchain-json)");
      }

      auto kind = values.front();
      String directory = values.size() > 1 ? values[1] : String(".");
      bool force = options.contains("force");

      generate(kind, std::filesystem::absolute(directory.c_str()).string(),
               force);

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

    -b|--build <directory>: Build project on directory containing package.cpp/package.json
    -i|--install <directory>: Install project on directory containing package.cpp/package.json
    -u|--uninstall <directory>: Uninstall project on directory containing package.cpp/package.json
    -g|--generate <kind> [directory]: Generate a starter manifest; kind is one of
                                        package-cpp, package-json, toolchain-cpp, toolchain-json
                                        (directory defaults to '.'; add --force to overwrite)

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

  void uninstall(const String &directory) {
    Utils::Unused{directory};
  }

  // Backs `cxpm --generate <kind> [directory]` (see docs/SRS-generate.md). `kind` selects both
  // the output filename and which serializer produces its content: the *-cpp kinds are
  // hand-written C++ source templates, the *-json kinds are rendered through the same
  // to_json()/write_json_pretty() path ProjectManager::load_project() reads back, so a
  // generated package.json is guaranteed to be a valid, buildable manifest.
  void generate(const String &kind, const String &directory, bool force) {
    if (!std::filesystem::exists(directory.c_str())) {
      std::filesystem::create_directories(directory.c_str());
    }

    auto default_name =
        String(std::filesystem::path(directory.c_str()).filename().string());
    if (default_name.empty()) {
      default_name = "example";
    }

    using namespace CXPM::Modules::Generators;

    String filename;
    String content;

    if (kind == "package-cpp") {
      filename = "package.cpp";
      content = generate_package_cpp_template(default_name);
    } else if (kind == "package-json") {
      filename = "package.json";
      content = generate_package_json_template(default_name);
    } else if (kind == "toolchain-cpp") {
      filename = "toolchain.cpp";
      content = generate_toolchain_cpp_template(default_name);
    } else if (kind == "toolchain-json") {
      filename = "toolchain.json";
      content = generate_toolchain_json_template(default_name);
    } else {
      throw CXPM::Core::Exceptions::RuntimeException(
          "Unknown --generate kind '{}': expected one of package-cpp, "
          "package-json, toolchain-cpp, toolchain-json",
          kind);
    }

    auto output_path =
        std::filesystem::path(directory.c_str()) / filename.c_str();

    if (std::filesystem::exists(output_path) && !force) {
      throw CXPM::Core::Exceptions::RuntimeException(
          "Refusing to overwrite existing file {} (pass --force to "
          "overwrite)",
          output_path.string());
    }

    std::ofstream stream(output_path,
                         std::ios_base::trunc | std::ios_base::out);
    if (!stream) {
      throw CXPM::Core::Exceptions::RuntimeException(
          "Couldn't open {} for writing", output_path.string());
    }
    stream << content.c_str();
    stream.flush();

    Core::Logging::LoggerManager::info("wrote {}", output_path.string());
  }

  void assert_project_directory(const String &directory) {
    if (!std::filesystem::exists(directory.c_str())) {
      throw CXPM::Core::Exceptions::RuntimeException(
          "Failed to build directory doesn't exists");
    }

    if (!std::filesystem::is_directory(directory.c_str())) {
      throw CXPM::Core::Exceptions::RuntimeException(
          "Failed to build folder isn't a directory");
    }

    // package.cpp and package.json are alternative serializations of the same manifest (see
    // docs/SRS-json-manifests.md); either is an acceptable project directory.
    auto cpp_project_file = directory + "/package.cpp";
    auto json_project_file = directory + "/package.json";

    bool has_cpp_manifest = std::filesystem::exists(cpp_project_file) &&
                            std::filesystem::is_regular_file(cpp_project_file);
    bool has_json_manifest =
        std::filesystem::exists(json_project_file) &&
        std::filesystem::is_regular_file(json_project_file);

    if (!has_cpp_manifest && !has_json_manifest) {
      throw Core::Exceptions::RuntimeException(
          "Failed to find package.cpp or package.json in {}", directory);
    }
  }
};
} // namespace Views