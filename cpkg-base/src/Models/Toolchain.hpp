#pragma once

#include "Core/Exceptions/NotImplementedException.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Models/BuildOutputResult.hpp"
#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/Project.hpp"
#include "Models/ToolchainDescriptorFactoryInterface.hpp"
#include "Models/ToolchainInterface.hpp"
#include "Utils/Unix/ShellManager.hpp"
#include <Controllers/PkgConfigManager.hpp>
#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Containers/Variant.hpp>
#include <Models/ProjectDescriptor.hpp>
#include <Models/ToolchainDescriptor.hpp>
#include <filesystem>
#include <future>
#include <memory>

using namespace Core::Containers;

namespace Models {

class Toolchain : public ToolchainDescriptorFactoryInterface<Toolchain>,
                  public ToolchainInterface {

public:
  using ToolchainInterface::ToolchainInterface;
  using ToolchainDescriptorFactoryInterface<
      Toolchain>::ToolchainDescriptorFactoryInterface;

  virtual ObjectBuildResult object_build(const String &source,
                                         const TargetDescriptor &target,
                                         bool dry = false) override {
    Core::Logging::LoggerManager::info("building {}: started", source.c_str());

    Collection<String> command;

    command.push_back(compiler_executable);

    command.append_range(target.options);

    command.push_back("-fPIC");

    auto include_directories_arguments =
        target.include_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{include_directory_prefix, el};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      command.append_range(include_directory_argument);
    }

    auto link_directories_arguments =
        target.link_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      command.append_range(link_directory_argument);
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      command.append_range(link_library_argument);
    }

    command.append_range(Core::Containers::Collection<Core::Containers::String>{
        source_specifier_prefix, source});

    command.append_range(
        Collection<String>({object_specifier_prefix, source + ".o"}));

    auto command_line = String::join(command, " ");

    Core::Logging::LoggerManager::debug("Calling: {}", command_line.c_str());
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::LoggerManager::info("building {}: ended", source.c_str());

    return {ObjectBuildResultStatus::Success,
            CompileCommandDescriptor{
                .directory = std::filesystem::current_path(),
                .command = command_line,
                .file = source,
                .output = source + ".o",
                .stdout = out,
                .stderr = err,
            }};
  }

  virtual ObjectBuildResultPromiseType
  object_build_async(const String &source,
                     const TargetDescriptor &target) override {
    return {};
  }

  virtual ExecutableLinkResult executable_link(const TargetDescriptor &target,
                                               bool dry = false) override {

    Collection<String> command;

    command.push_back(compiler_executable);

    command.append_range(target.options);

    command.push_back("-fPIE");

    auto include_directories_arguments =
        target.include_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{include_directory_prefix, el};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      command.append_range(include_directory_argument);
    }

    auto link_directories_arguments =
        target.link_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      command.append_range(link_directory_argument);
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      command.append_range(link_library_argument);
    }

    command.append_range(Collection<String>{"-o", target.name});

    for (auto source : target.sources) {
      command.push_back(source + ".o");
    }

    auto command_line = String::join(command, " ");

    Core::Logging::LoggerManager::debug("Calling: {}", command_line.c_str());
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;

    exec_result = Utils::Unix::ShellManager::exec(command_line, dry);

    Core::Logging::LoggerManager::info("building: ended");

    return {ExecutableLinkResultStatus::Success,
            CompileCommandDescriptor{
                .directory = std::filesystem::current_path(),
                .command = command_line,
                .file = "",
                .output = target.name,
                .stdout = out,
                .stderr = err,
            }};
  }

  virtual ExecutableLinkResultPromise
  executable_link_async(const TargetDescriptor & /*target*/) override {
    return ExecutableLinkResultPromise();
  }

  virtual SharedObjectLinkResult
  shared_object_link(const TargetDescriptor &target, bool dry = false,
                     const String &library_prefix = "lib") override {

    Collection<String> command;

    command.push_back(compiler_executable);

    command.push_back("-fPIC");
    command.push_back("-shared");

    command.append_range(this->linker_options);

    command.append_range(target.options);

    auto include_directories_arguments =
        target.include_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{include_directory_prefix, el};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      command.append_range(include_directory_argument);
    }

    auto link_directories_arguments =
        target.link_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      command.append_range(link_directory_argument);
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      command.append_range(link_library_argument);
    }

    command.append_range(
        Collection<String>{"-o", library_prefix + target.name + ".so"});

    for (auto source : target.sources) {
      command.push_back(source + ".o");
    }

    auto command_line = String::join(command, " ");

    Core::Logging::LoggerManager::debug("Calling: {}", command_line.c_str());
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::LoggerManager::info("building: ended");

    return {SharedObjectLinkResultStatus::Success,
            CompileCommandDescriptor{
                .directory = std::filesystem::current_path(),
                .command = command_line,
                .file = "",
                .output = library_prefix + target.name + ".so",
                .stdout = out,
                .stderr = err,
            }};
  }

  virtual ArchiveLinkResult
  archive_link(const TargetDescriptor &target, bool dry = false,
               const String &library_prefix = "lib") override {

    Collection<String> command;

    command.push_back(archiver_executable);

    command.append_range(this->archiver_options);

    command.append_range(
        Collection<String>{"-o", library_prefix + target.name + ".a"});

    for (auto source : target.sources) {
      command.push_back(source + ".o");
    }

    auto command_line = String::join(command, " ");

    Core::Logging::LoggerManager::debug("Calling: {}", command_line.c_str());
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::LoggerManager::info("building: ended");

    return {ArchiverLinkResultStatus::Success,
            CompileCommandDescriptor{
                .directory = std::filesystem::current_path(),
                .command = command_line,
                .file = "",
                .output = target.name + ".a",
                .stdout = out,
                .stderr = err,
            }};
  }

  virtual BuildResult build(const ProjectDescriptor &project,
                            bool dry = false) override {

    BuildResult retval;
    auto &[code, result] = retval;

    for (auto package : project.targets) {
      const auto &[current_code, current_result] = build(package, dry);

      code = current_code;

      result.append_range(current_result);

      if (code != 0) {
        break;
      }
    }

    return retval;
  }

  virtual BuildResult build(const TargetDescriptor &input,
                            bool dry = false) override {

    auto package = input;

    BuildResult result;
    auto &[result_code, result_commands] = result;

    using promise_type =
        std::shared_ptr<std::promise<std::tuple<int, String, String>>>;

    for (auto dependency : package.dependencies) {

      auto result = Controllers::PackageConfigManager::find_package(dependency);
      package.include_directories.append_range(result.include_directories);
      package.link_directories.append_range(result.link_directories);
      package.link_libraries.append_range(result.link_libraries);
      package.options.append_range(result.options);
    }

    std::deque<std::tuple<String, promise_type>> results;
    for (auto source : package.sources) {
      auto [code, commands] = object_build(source, package, dry);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ObjectBuildResultStatus::Success) {
        return result;
      }
    }

    if (package.type == "executable") {
      auto [code, commands] = executable_link(package, dry);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ExecutableLinkResultStatus::Success) {
        return result;
      }
    }

    if (package.type == "shared-library") {
      auto [code, commands] = shared_object_link(package);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != SharedObjectLinkResultStatus::Success) {
        return result;
      }
    }

    if (package.type == "static-library") {
      auto [code, commands] = archive_link(package, dry);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ArchiverLinkResultStatus::Success) {
        return result;
      }
    }

    return result;
  }

  virtual int install(const Models::ProjectDescriptor &target) override {
    throw Core::Exceptions::NotImplementedException();
    return 0;
  }

  virtual int install(const Models::TargetDescriptor &target) override {
    throw Core::Exceptions::NotImplementedException();
    return 0;
  }
};

} // namespace Models