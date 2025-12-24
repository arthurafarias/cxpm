#pragma once

#include "Core/Exceptions/NotImplementedException.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Models/BuildOutputResult.hpp"
#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/ToolchainArchiveLinkInterface.hpp"
#include "Models/ToolchainExecutableLinkInterface.hpp"
#include "Models/ToolchainObjectBuildInterface.hpp"
#include "Models/ToolchainSharedObjectLinkInterface.hpp"
#include "Utils/Unix/ShellManager.hpp"
#include <Controllers/PkgConfigManager.hpp>
#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Containers/Variant.hpp>
#include <Models/ProjectDescriptor.hpp>
#include <Models/ToolchainDescriptor.hpp>
#include <algorithm>
#include <filesystem>
#include <future>
#include <memory>

using namespace Core::Containers;

namespace Models {

class Toolchain : public ToolchainDescriptor,
                  public ToolchainObjectBuildInterface,
                  public ToolchainSharedObjectLinkInterface,
                  public ToolchainArchiveLinkInterface,
                  public ToolchainExecutableLinkInterface {

public:
  Toolchain() {}
  Toolchain(const ToolchainDescriptor &descriptor)
      : ToolchainDescriptor(descriptor) {}
  virtual ObjectBuildResult object_build(const String &source,
                                         const TargetDescriptor &target,
                                         bool dry = false) override {
    Core::Logging::LoggerManager::info("building {}: started", source);

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

    Core::Logging::LoggerManager::debug("Calling: {}", command_line);
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::LoggerManager::info("building {}: ended", source);

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

  ObjectBuildResultPromiseType
  object_build_async(const String &source, const TargetDescriptor &target,
                     bool dry) override {
    return std::async(std::launch::async,
                      [&]() { return this->object_build(source, target, dry); })
        .share();
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

    Core::Logging::LoggerManager::debug("Calling: {}", command_line);
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

  virtual ExecutableLinkResultPromiseType
  executable_link_async(const TargetDescriptor &target) override {
    return std::async(std::launch::async,
                      [&]() { return executable_link(target); })
        .share();
  }

  virtual SharedObjectLinkResultPromiseType
  shared_object_link_async(const TargetDescriptor &target, bool dry = false,
                           const String &library_prefix = "lib") override {
    return std::async(std::launch::async,
                      [&]() {
                        return shared_object_link(target, dry, library_prefix);
                      })
        .share();
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

    Core::Logging::LoggerManager::debug("Calling: {}", command_line);
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

  virtual ArchiveLinkResultPromiseType
  archive_link_async(const TargetDescriptor &target, bool dry = false,
                     const String &library_prefix = "lib") override {
    return std::async(
               std::launch::async,
               [&]() { return archive_link(target, dry, library_prefix); })
        .share();
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

    Core::Logging::LoggerManager::debug("Calling: {}", command_line);
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

  virtual BuildOutputResult build(const ProjectDescriptor &project,
                                  bool dry = false) {

    BuildOutputResult retval;
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

  virtual BuildOutputResult build(const TargetDescriptor &input,
                                  bool dry = false) {

    auto package = input;

    BuildOutputResult build_result = {BuildOutputResultStatus::Failure, {}};
    auto &[result_code, result_commands] = build_result;

    using promise_type =
        std::shared_ptr<std::promise<std::tuple<int, String, String>>>;

    for (auto dependency : package.dependencies) {

      auto result = Controllers::PackageConfigManager::find_package(dependency);
      package.include_directories.append_range(result.include_directories);
      package.link_directories.append_range(result.link_directories);
      package.link_libraries.append_range(result.link_libraries);
      package.options.append_range(result.options);
    }

    std::deque<ObjectBuildResultPromiseType> results;

    for (auto source : package.sources) {
      results.push_back(object_build_async(source, package, dry));
    };

    for (auto result : results) {
      auto [code, commands] = result.get();
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ObjectBuildResultStatus::Success) {
        return build_result;
      }
    }

    if (package.type == "executable") {
      auto [code, commands] = executable_link(package, dry);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ExecutableLinkResultStatus::Success) {
        return build_result;
      }
    }

    if (package.type == "shared-library") {
      auto [code, commands] = shared_object_link(package);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != SharedObjectLinkResultStatus::Success) {
        return build_result;
      }
    }

    if (package.type == "static-library") {
      auto [code, commands] = archive_link(package, dry);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ArchiverLinkResultStatus::Success) {
        return build_result;
      }
    }

    return build_result;
  }

  virtual int install(const Models::ProjectDescriptor &target) {
    throw Core::Exceptions::NotImplementedException();
    return 0;
  }

  virtual int install(const Models::TargetDescriptor &target) {
    throw Core::Exceptions::NotImplementedException();
    return 0;
  }
};

} // namespace Models