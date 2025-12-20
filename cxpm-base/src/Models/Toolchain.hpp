#pragma once

#include "Core/Exceptions/NotImplementedException.hpp"
#include "Core/Logging/Manager.hpp"
#include "Models/BuildOutputResult.hpp"
#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/ToolchainDescriptorFactoryInterface.hpp"
#include "Models/ToolchainInterface.hpp"
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
#include <iterator>
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
    Core::Logging::Logger::info("building {}: started", source.c_str());

    Collection<String> command;

    command.push_back(compiler_executable);

    std::copy(target.options.begin(), target.options.end(),
              std::back_inserter(command));

    command.push_back("-fPIC");

    auto include_directories_arguments =
        target.include_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{include_directory_prefix, el};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      std::copy(include_directory_argument.begin(),
                include_directory_argument.end(), std::back_inserter(command));
    }

    auto link_directories_arguments =
        target.link_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      std::copy(link_directory_argument.begin(), link_directory_argument.end(),
                std::back_inserter(command));
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      std::copy(link_library_argument.begin(), link_library_argument.end(),
                std::back_inserter(command));
    }

    {
      auto elements = Core::Containers::Collection<Core::Containers::String>{
          source_specifier_prefix, source};
      std::copy(elements.begin(), elements.end(), std::back_inserter(command));
    }

    {
      auto elements =
          Collection<String>({object_specifier_prefix, source + ".o"});
      std::copy(elements.begin(), elements.end(), std::back_inserter(command));
    }

    auto command_line = String::join(command, " ");

    Core::Logging::Logger::debug("Calling: {}", command_line.c_str());
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::Logger::info("building {}: ended", source.c_str());

    return {ObjectBuildResultStatus::Success,
            CompileCommandDescriptor{std::filesystem::current_path(),
                                     command_line, source, source + ".o", out,
                                     err}};
  }

  virtual ObjectBuildResultPromiseType
  object_build_async(const String &source,
                     const TargetDescriptor &target) override {
    UNUSED(source);
    UNUSED(target);
    return {};
  }

  virtual ExecutableLinkResult executable_link(const TargetDescriptor &target,
                                               bool dry = false) override {

    Collection<String> command;

    command.push_back(compiler_executable);

    std::copy(target.options.begin(), target.options.end(),
              std::back_inserter(command));

    command.push_back("-fPIE");

    auto include_directories_arguments =
        target.include_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{include_directory_prefix, el};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      std::copy(include_directory_argument.begin(),
                include_directory_argument.end(),
                std::back_inserter(include_directory_argument));
    }

    auto link_directories_arguments =
        target.link_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      std::copy(link_directory_argument.begin(), link_directory_argument.end(),
                std::back_inserter(command));
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      std::copy(link_library_argument.begin(), link_library_argument.end(),
                std::back_inserter(link_library_argument));
    }

    {
      auto elements = Collection<String>{"-o", target.name};
      std::copy(elements.begin(), elements.end(), std::back_inserter(command));
    }

    for (auto source : target.sources) {
      command.push_back(source + ".o");
    }

    auto command_line = String::join(command, " ");

    Core::Logging::Logger::debug("Calling: {}", command_line.c_str());
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;

    exec_result = Utils::Unix::ShellManager::exec(command_line, dry);

    Core::Logging::Logger::info("building: ended");

    return {ExecutableLinkResultStatus::Success,
            CompileCommandDescriptor{std::filesystem::current_path(),
                                     command_line, "", target.name, out, err}};
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

    std::copy(this->linker_options.begin(), this->linker_options.end(),
              std::back_inserter(command));

    std::copy(target.options.begin(), target.options.end(),
              std::back_inserter(command));

    auto include_directories_arguments =
        target.include_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{include_directory_prefix, el};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      std::copy(include_directory_argument.begin(),
                include_directory_argument.end(), std::back_inserter(command));
    }

    auto link_directories_arguments =
        target.link_directories.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      std::copy(link_directory_argument.begin(), link_directory_argument.end(),
                std::back_inserter(command));
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<Collection<String>>(
            [this](const auto &el) {
              return Collection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      std::copy(link_library_argument.begin(), link_library_argument.end(),
                std::back_inserter(command));
    }

    {
      auto elements =
          Collection<String>{"-o", library_prefix + target.name + ".so"};
      std::copy(elements.begin(), elements.end(), std::back_inserter(command));
    }

    for (auto source : target.sources) {
      command.push_back(source + ".o");
    }

    auto command_line = String::join(command, " ");

    Core::Logging::Logger::debug("Calling: {}", command_line.c_str());
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::Logger::info("building: ended");

    return {SharedObjectLinkResultStatus::Success,
            CompileCommandDescriptor{
                std::filesystem::current_path(),
                command_line,
                "",
                library_prefix + target.name + ".so",
                out,
                err,
            }};
  }

  virtual ArchiveLinkResult
  archive_link(const TargetDescriptor &target, bool dry = false,
               const String &library_prefix = "lib") override {

    Collection<String> command;

    command.push_back(archiver_executable);

    std::copy(this->archiver_options.begin(), this->archiver_options.end(),
              std::back_inserter(this->archiver_options));

    {
      auto elements =
          Collection<String>{"-o", library_prefix + target.name + ".a"};
      std::copy(elements.begin(), elements.end(), std::back_inserter(command));
    }

    for (auto source : target.sources) {
      command.push_back(source + ".o");
    }

    auto command_line = String::join(command, " ");

    Core::Logging::Logger::debug("Calling: {}", command_line.c_str());
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::Logger::info("building: ended");

    return {ArchiverLinkResultStatus::Success,
            CompileCommandDescriptor{
                std::filesystem::current_path(),
                command_line,
                "",
                target.name + ".a",
                out,
                err,
            }};
  }

  virtual BuildResult build(const ProjectDescriptor &project,
                            bool dry = false) override {

    BuildResult retval;
    auto &[code, result] = retval;

    for (auto package : project.targets) {
      const auto &[current_code, current_result] = build(package, dry);

      code = current_code;

      std::copy(current_result.begin(), current_result.end(), std::back_inserter(result));

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
      std::copy(result.include_directories.begin(), result.include_directories.end(), std::back_inserter(package.include_directories));
      std::copy(result.link_directories.begin(), result.link_directories.end(), std::back_inserter(package.link_directories));
      std::copy(result.link_libraries.begin(), result.link_libraries.end(), std::back_inserter(package.link_libraries));
      std::copy(result.options.begin(), result.options.end(), std::back_inserter(package.options));
    }

    std::deque<std::tuple<String, promise_type>> results;
    for (auto source : package.sources) {
      auto [code, commands] = object_build(source, package, dry);
      Core::Logging::Logger::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ObjectBuildResultStatus::Success) {
        return result;
      }
    }

    if (package.type == "executable") {
      auto [code, commands] = executable_link(package, dry);
      Core::Logging::Logger::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ExecutableLinkResultStatus::Success) {
        return result;
      }
    }

    if (package.type == "shared-library") {
      auto [code, commands] = shared_object_link(package);
      Core::Logging::Logger::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != SharedObjectLinkResultStatus::Success) {
        return result;
      }
    }

    if (package.type == "static-library") {
      auto [code, commands] = archive_link(package, dry);
      Core::Logging::Logger::debug("{}", std::to_string(code));
      result_commands.push_back(commands);
      if (code != ArchiverLinkResultStatus::Success) {
        return result;
      }
    }

    return result;
  }

  virtual int install(const Models::ProjectDescriptor &target) override {
    UNUSED(target);
    throw Core::Exceptions::NotImplementedException();
    return 0;
  }

  virtual int install(const Models::TargetDescriptor &target) override {
    UNUSED(target);
    throw Core::Exceptions::NotImplementedException();
    return 0;
  }
};

} // namespace Models