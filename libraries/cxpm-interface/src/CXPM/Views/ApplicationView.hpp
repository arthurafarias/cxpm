#pragma once

#include "CXPM/Core/Exceptions/RuntimeException.hpp"
#include "CXPM/Core/Logging/LoggerManager.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/Modules/Console/AbstractConsoleApplication.hpp"
#include "CXPM/Modules/Generators/ManifestGenerator.hpp"
#include "CXPM/Modules/ProgramOptions/CommandLineParser.hpp"
#include "CXPM/Modules/ProgramOptions/CommandRegistry.hpp"
#include "CXPM/Modules/ProgramOptions/HelpFormatter.hpp"
#include "CXPM/Utils/Unused.hpp"
#include <filesystem>
#include <fstream>

#include <CXPM/ProjectManager.hpp>

using namespace CXPM::Modules::Console;
using namespace CXPM::Modules::ProgramOptions;

namespace CXPM::Views {
class ApplicationView final : public AbstractConsoleApplication {
public:
  ApplicationView(int argc, char *argv[])
      : AbstractConsoleApplication(argc, argv) {}

  // A git-like CLI: `cxpm <command> [<args>]`. See docs/srs-cli-subcommands.md for the full
  // contract this dispatch implements.
  virtual int run() override {
    auto lk = acquire_lock();

    setup();

    auto arguments = args();
    // AbstractApplication seeds args() straight from argv, including argv[0] (the program
    // path); CommandLineParser::parse expects that already stripped, exactly like argv + 1.
    BasicCollection<String> tokens(arguments.begin() + 1, arguments.end());

    if (tokens.empty()) {
      print_usage();
      return 1;
    }

    auto registry = command_registry();
    CommandLineParser parser(registry);
    auto parsed = parser.parse(tokens);

    if (parsed.command == "help") {
      if (!parsed.positionals.empty()) {
        print_command_usage(registry, parsed.positionals.front());
      } else {
        print_usage(registry);
      }
      return 0;
    }

    if (parsed.has_option("help")) {
      print_command_usage(registry, parsed.command);
      return 0;
    }

    if (parsed.command == "build") {

      if (parsed.positionals.empty()) {
        throw CXPM::Core::Exceptions::RuntimeException(
            "Failed to build: no directory supplied");
      }

      auto directory = parsed.positionals.front();

      assert_project_directory(directory);

      build(std::filesystem::absolute(directory.c_str()));

      return 0;
    }

    if (parsed.command == "install") {

      if (parsed.positionals.empty()) {
        throw CXPM::Core::Exceptions::RuntimeException(
            "Failed to install: no directory supplied");
      }

      auto directory = parsed.positionals.front();
      auto prefix = parsed.option_value("prefix");

      assert_project_directory(directory);

      auto [status, project] = install_project(std::filesystem::absolute(directory.c_str()), prefix);

      switch (status) {
      case Status::Failure:
        return 1;
      case Status::Success:
        break;
      }

      return 0;
    }

    if (parsed.command == "generate") {

      if (parsed.positionals.empty()) {
        throw CXPM::Core::Exceptions::RuntimeException(
            "Failed to generate: no kind supplied (expected one of "
            "package-cpp, package-json, toolchain-cpp, toolchain-json)");
      }

      auto kind = parsed.positionals.front();
      String directory = parsed.positionals.size() > 1 ? parsed.positionals[1] : String(".");
      bool force = parsed.has_option("force");

      generate(kind, std::filesystem::absolute(directory.c_str()).string(),
               force);

      return 0;
    }

    print_usage(registry);

    return 1;
  }

  void print_usage() { print_usage(command_registry()); }

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
  // The declarative schema every subcommand is parsed and rendered against -- see
  // docs/srs-cli-subcommands.md. Adding a subcommand only ever means adding one entry here.
  static CommandRegistry command_registry() {
    CommandRegistry registry;

    registry.push_back({"build",
                        "Build a project in a directory containing package.cpp/package.json",
                        "<directory>", OptionsDescriptorCollection("build", "")});

    OptionsDescriptorCollection install_options("install", "");
    install_options.push_back({"prefix", "p", "/usr/local", "Install prefix"});
    registry.push_back({"install", "Build a project, then install it into a prefix",
                        "<directory>", install_options});

    OptionsDescriptorCollection generate_options("generate", "");
    generate_options.push_back(
        {"force", "f", "", "Overwrite the target file if it already exists"});
    registry.push_back({"generate",
                        "Scaffold a starter package/toolchain manifest",
                        "<kind> [directory]", generate_options});

    registry.push_back({"help", "Show this message, or detailed help for one command",
                        "[<command>]", OptionsDescriptorCollection("help", "")});

    return registry;
  }

  void print_usage(const CommandRegistry &registry) {
    std::osyncstream(std::cout)
        << HelpFormatter::top_level("cxpm", "A package manager for C++ using C++", registry);
  }

  void print_command_usage(const CommandRegistry &registry, const String &name) {
    auto *descriptor = registry.find(name);

    if (descriptor == nullptr) {
      throw CXPM::Core::Exceptions::RuntimeException(
          "cxpm: '{}' is not a cxpm command. See 'cxpm help'.", name);
    }

    std::osyncstream(std::cout) << HelpFormatter::command("cxpm", *descriptor);
  }

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
      // ProjectManager::build_project's own target loop iterates by value, so the
      // project_path it stamps onto each target for the build never makes it back onto
      // project.targets here -- every target below would otherwise have an empty
      // project_path, making ProjectManager::install_target unable to resolve any of the
      // target's relative include directories or its own built artifact (a real bug found
      // via the cli_install_example_executable integration test, see
      // docs/SRS-architecture.md).
      target.project_path = directory;

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