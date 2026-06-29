#pragma once

#include "CXPM/BuildOutputResult.hpp"
#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/Core/Containers/Collection.hpp"
#include "CXPM/Core/Containers/String.hpp"
#include "CXPM/Core/Exceptions/NotImplementedException.hpp"
#include "CXPM/Core/Logging/LoggerManager.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/ToolchainDescriptor.hpp"
#include "CXPM/ToolchainInterface.hpp"
#include "CXPM/Utils/Unix/ShellManager.hpp"
#include "CXPM/Utils/Unused.hpp"
#include <CXPM/PkgConfigManager.hpp>

#include <algorithm>
#include <filesystem>
#include <future>
#include <memory>

using namespace CXPM::Core::Containers;

namespace CXPM::Models {

struct Toolchain : public ToolchainDescriptor,
                   public ToolchainInterface<Toolchain> {

  Toolchain() {}
  Toolchain(const ToolchainDescriptor &descriptor)
      : ToolchainDescriptor(descriptor) {}

  virtual Toolchain &name_set(const String &value) override {
    name = value;
    return *this;
  }

  virtual const String &name_get() const override { return name; }

  virtual Toolchain &version_set(const String &value) override {
    version = value;
    return *this;
  }

  virtual const String &version_get() const override { return version; }

  virtual Toolchain &language_set(const String &value) override {
    language = value;
    return *this;
  }

  virtual const String &language_get() const override { return language; }

  virtual Toolchain &install_prefix_set(const String &value) override {
    install_prefix = value;
    return *this;
  }

  virtual const String &install_prefix_get() const override {
    return install_prefix;
  }

  virtual Toolchain &
  include_directory_prefix_set(const String &value) override {
    include_directory_prefix = value;
    return *this;
  }

  virtual const String &include_directory_prefix_get() const override {
    return include_directory_prefix;
  }

  virtual Toolchain &
  include_directories_set(const BasicCollection<String> &value) override {
    include_directories = value;
    return *this;
  }
  virtual const BasicCollection<String> &include_directories_get() const override {
    return include_directories;
  }

  virtual Toolchain &compiler_executable_set(const String &value) override {
    compiler_executable = value;
    return *this;
  }

  virtual const String &compiler_executable_get() const override {
    return compiler_executable;
  }

  virtual Toolchain &
  object_build_options_set(const BasicCollection<String> &value) override {
    object_build_options = value;
    return *this;
  }

  virtual const BasicCollection<String> &object_build_options_get() const override {
    return object_build_options;
  }

  virtual Toolchain &
  compiler_options_set(const BasicCollection<String> &value) override {
    compiler_options = value;
    return *this;
  }

  virtual const BasicCollection<String> &compiler_options_get() const override {
    return compiler_options;
  }

  virtual Toolchain &archiver_executable_set(const String &value) override {
    archiver_executable = value;
    return *this;
  }

  virtual const String &archiver_executable_get() const override {
    return archiver_executable;
  }

  virtual Toolchain &
  archiver_options_set(const BasicCollection<String> &value) override {
    archiver_options = value;
    return *this;
  }

  virtual const BasicCollection<String> &archiver_options_get() const override {
    return archiver_options;
  }

  virtual Toolchain &linker_executable_set(const String &value) override {
    linker_executable = value;
    return *this;
  }

  virtual const String &linker_executable_get() const override {
    return linker_executable;
  }

  virtual Toolchain &
  linker_options_set(const BasicCollection<String> &value) override {
    linker_options = value;
    return *this;
  }

  virtual const BasicCollection<String> &linker_options_get() const override {
    return linker_options;
  }

  virtual Toolchain &link_directory_prefix_set(const String &value) override {
    link_directory_prefix = value;
    return *this;
  }
  virtual const String &link_directory_prefix_get() const override {
    return link_directory_prefix;
  }

  virtual Toolchain &link_library_prefix_set(const String &value) override {
    link_library_prefix = value;
    return *this;
  }

  virtual const String &link_library_prefix_get() const override {
    return link_library_prefix;
  }

  virtual Toolchain &source_specifier_prefix_set(const String &value) override {
    source_specifier_prefix = value;
    return *this;
  }

  virtual const String &source_specifier_prefix_get() const override {
    return source_specifier_prefix;
  }

  virtual Toolchain &object_specifier_prefix_set(const String &value) override {
    object_specifier_prefix = value;
    return *this;
  }

  virtual const String &object_specifier_prefix_get() const override {
    return object_specifier_prefix;
  }

  virtual Toolchain &
  link_directories_set(const BasicCollection<String> &value) override {
    link_directories = value;
    return *this;
  }

  virtual const BasicCollection<String> &link_directories_get() const override {
    return link_directories;
  }

  virtual Toolchain &shared_object_prefix_set(const String &value) override {
    shared_object_prefix = value;
    return *this;
  }

  virtual const String &shared_object_prefix_get() const override {
    return shared_object_prefix;
  }

  virtual Toolchain &shared_object_suffix_set(const String &value) override {
    shared_object_suffix = value;
    return *this;
  }
  virtual const String &shared_object_suffix_get() const override {
    return shared_object_suffix;
  }

  virtual Toolchain &archive_prefix_set(const String &value) override {
    archive_prefix = value;
    return *this;
  }
  virtual const String &archive_prefix_get() const override {
    return archive_prefix;
  }

  virtual Toolchain &archive_suffix_set(const String &value) override {
    archive_suffix = value;
    return *this;
  }
  virtual const String &archive_suffix_get() const override {
    return archive_suffix;
  }

  virtual Toolchain &executable_prefix_set(const String &value) override {
    executable_prefix = value;
    return *this;
  }
  virtual const String &executable_prefix_get() const override {
    return executable_prefix;
  }

  virtual Toolchain &executable_suffix_set(const String &value) override {
    executable_suffix = value;
    return *this;
  }
  virtual const String &executable_suffix_get() const override {
    return executable_suffix;
  }

  virtual Toolchain &object_prefix_set(const String &value) override {
    object_prefix = value;
    return *this;
  }
  virtual const String &object_prefix_get() const override {
    return object_prefix;
  }

  virtual Toolchain &object_suffix_set(const String &value) override {
    object_suffix = value;
    return *this;
  }
  virtual const String &object_suffix_get() const override {
    return object_suffix;
  }

  virtual ObjectBuildResult object_build(const String &source,
                                         const TargetDescriptor &target,
                                         bool dry = false) override {

    Core::Logging::LoggerManager::info("building {}: started", source);

    BasicCollection<String> command;

    command.push_back(compiler_executable);

    command.append_range(object_build_options);
    command.append_range(target.options);

    auto include_directories_arguments =
        target.include_directories.transform<BasicCollection<String>>(
            [this, &target](const auto &el) {
              auto path = std::filesystem::path(el.c_str());

              if (!path.is_absolute()) {
                path = std::filesystem::path()
                           .append(target.project_path.c_str())
                           .append(el.c_str());
              }

              return BasicCollection<String>{
                  include_directory_prefix,
                  std::filesystem::absolute(path).string()};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      command.append_range(include_directory_argument);
    }

    auto link_directories_arguments =
        target.link_directories.transform<BasicCollection<String>>(
            [this](const auto &el) {
              return BasicCollection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      command.append_range(link_directory_argument);
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<BasicCollection<String>>(
            [this](const auto &el) {
              return BasicCollection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      command.append_range(link_library_argument);
    }

    auto source_path = std::filesystem::path()
                           .append(target.project_path.c_str())
                           .append(source.c_str());

    auto object_path =
        std::filesystem::path()
            .append(target.project_path.c_str())
            .append(object_prefix + source_path.string() + object_suffix);

    command.append_range(BasicCollection<String>{
        source_specifier_prefix, source_path.string()});

    command.append_range(
        BasicCollection<String>({object_specifier_prefix, object_path.string()}));

    auto command_line = String::join(command, " ");

    Core::Logging::LoggerManager::debug("Calling: {}", command_line);
    std::tuple<int, String, String> exec_result;

    const auto &[result_code, out, err] = exec_result;

    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }

    Core::Logging::LoggerManager::info("building {}: ended", source);

    auto status = result_code != 0 ? Status::Failure : Status::Success;

    return {status, CompileCommandDescriptor{
                        .directory = std::filesystem::current_path(),
                        .command = command_line,
                        .file = source,
                        .output = object_prefix + source + object_suffix,
                        .stdout = out,
                        .stderr = err,
                    }};
  }

  ObjectBuildResultPromiseType
  object_build_async(const String &source, const TargetDescriptor &target,
                     bool dry) override {

    return std::async(std::launch::async,
                      [this, source, target, dry]() mutable {
                        return this->object_build(source, target, dry);
                      })
        .share();
  }

  virtual ExecutableLinkResult executable_link(const TargetDescriptor &target,
                                               bool dry = false) override {

    BasicCollection<String> command;

    command.push_back(compiler_executable);

    command.append_range(target.options);

    auto include_directories_arguments =
        target.include_directories.transform<BasicCollection<String>>(
            [this](const auto &el) {
              return BasicCollection<String>{include_directory_prefix, el};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      command.append_range(include_directory_argument);
    }

    auto link_directories_arguments =
        target.link_directories.transform<BasicCollection<String>>(
            [this](const auto &el) {
              return BasicCollection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      command.append_range(link_directory_argument);
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<BasicCollection<String>>(
            [this](const auto &el) {
              return BasicCollection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      command.append_range(link_library_argument);
    }

    auto target_path = std::filesystem::path()
                           .append(target.project_path.c_str())
                           .append(target.name.c_str());

    command.append_range(BasicCollection<String>{"-o", target_path.string()});

    for (auto source : target.sources) {

      auto source_path = std::filesystem::path()
                             .append(target.project_path.c_str())
                             .append(source.c_str())
                             .string() +
                         object_suffix;

      command.push_back(std::filesystem::absolute(source_path));
    }

    auto command_line = String::join(command, " ");

    Core::Logging::LoggerManager::debug("Calling: {}", command_line);

    const auto &[result_code, out, err] =
        Utils::Unix::ShellManager::exec(command_line, dry);

    Core::Logging::LoggerManager::info("building: ended");

    auto status = result_code != 0 ? Status::Failure : Status::Success;

    return {status, CompileCommandDescriptor{
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

    return std::async(
               std::launch::async,
               [this, target = target]() { return executable_link(target); })
        .share();
  }

  virtual SharedObjectLinkResultPromiseType
  shared_object_link_async(const TargetDescriptor &target, bool dry = false,
                           const String &library_prefix = "lib") override {
    return std::async(std::launch::async,
                      [this, target, dry, library_prefix]() {
                        return shared_object_link(target, dry, library_prefix);
                      })
        .share();
  }

  virtual SharedObjectLinkResult
  shared_object_link(const TargetDescriptor &target, bool dry = false,
                     const String &library_prefix = "lib") override {

    BasicCollection<String> command;

    command.push_back(compiler_executable);

    command.append_range(this->linker_options);

    command.append_range(target.options);

    auto include_directories_arguments =
        target.include_directories.transform<BasicCollection<String>>(
            [this](const auto &el) {
              return BasicCollection<String>{include_directory_prefix, el};
            });

    for (auto include_directory_argument : include_directories_arguments) {
      command.append_range(include_directory_argument);
    }

    auto link_directories_arguments =
        target.link_directories.transform<BasicCollection<String>>(
            [this](const auto &el) {
              return BasicCollection<String>{link_directory_prefix, el};
            });

    for (auto link_directory_argument : link_directories_arguments) {
      command.append_range(link_directory_argument);
    }

    auto link_libraries_arguments =
        target.link_libraries.transform<BasicCollection<String>>(
            [this](const auto &el) {
              return BasicCollection<String>{link_library_prefix, el};
            });

    for (auto link_library_argument : link_libraries_arguments) {
      command.append_range(link_library_argument);
    }

    auto target_path = std::filesystem::path()
                           .append(target.project_path.c_str())
                           .append(shared_object_prefix + target.name.c_str() +
                                   shared_object_suffix);

    command.append_range(BasicCollection<String>{"-o", target_path.string()});

    for (auto source : target.sources) {

      auto source_path = std::filesystem::path()
                             .append(target.project_path.c_str())
                             .append(object_prefix + source + object_suffix);

      command.push_back(std::filesystem::absolute(source_path));
    }

    auto command_line = String::join(command, " ");

    Core::Logging::LoggerManager::debug("Calling: {}", command_line);
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::LoggerManager::info("building: ended");

    return {Status::Success, CompileCommandDescriptor{
                                 .directory = std::filesystem::current_path(),
                                 .command = command_line,
                                 .file = "",
                                 .output = library_prefix + target.name + ".so",
                                 .stdout = out,
                                 .stderr = err,
                             }};
  }

  virtual ArchiveLinkResultPromiseType
  archive_link_async(const TargetDescriptor &target,
                     bool dry = false) override {
    return std::async(std::launch::async,
                      [&]() { return archive_link(target, dry); })
        .share();
  }

  virtual ArchiveLinkResult archive_link(const TargetDescriptor &target,
                                         bool dry = false) override {

    BasicCollection<String> command;

    command.push_back(archiver_executable);

    command.append_range(this->archiver_options);

    auto target_path =
        std::filesystem::path()
            .append(target.project_path.c_str())
            .append(archive_prefix_get() + target.name + archive_suffix_get());

    command.append_range(BasicCollection<String>{"-o", target_path.string()});

    for (auto source : target.sources) {

      auto source_path = std::filesystem::path()
                             .append(target.project_path.c_str())
                             .append(object_prefix + source + object_suffix);

      command.push_back(std::filesystem::absolute(source_path));
    }

    auto command_line = String::join(command, " ");

    Core::Logging::LoggerManager::debug("Calling: {}", command_line);
    std::tuple<int, String, String> exec_result;
    const auto &[result_code, out, err] = exec_result;
    if (!dry) {
      exec_result = Utils::Unix::ShellManager::exec(command_line, dry);
    }
    Core::Logging::LoggerManager::info("building: ended");

    return {Status::Success, CompileCommandDescriptor{
                                 .directory = std::filesystem::current_path(),
                                 .command = command_line,
                                 .file = "",
                                 .output = target.name + ".a",
                                 .stdout = out,
                                 .stderr = err,
                             }};
  }

  virtual BuildOutputResult build(const ProjectDescriptor &project,
                                  bool dry = false) override {

    BuildOutputResult retval;
    auto &[code, result] = retval;

    for (auto package : project.targets) {
      const auto &[current_code, current_result] = build(package, dry);

      code = current_code;

      result.append_range(current_result);

      if (code != Status::Success) {
        break;
      }
    }

    return retval;
  }

  virtual BuildOutputResult build(const TargetDescriptor &input,
                                  bool dry = false) override {

    auto package = input;

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
      auto [code, command] = result.get();
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      if (code != Status::Success) {
        return BuildOutputResult{code, {command}};
      }
    }

    if (package.type == "executable") {
      auto [code, command] = executable_link(package, dry);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      return BuildOutputResult{code, {command}};
    }

    if (package.type == "shared-library") {
      auto [code, command] = shared_object_link(package);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      return BuildOutputResult{code, {command}};
    }

    if (package.type == "static-library") {
      auto [code, command] = archive_link(package, dry);
      Core::Logging::LoggerManager::debug("{}", std::to_string(code));
      return BuildOutputResult{code, {command}};
    }

    /** This is an special case where we are trying to build an unknown target
     */
    return BuildOutputResult{Status::Failure, {}};
  }

  virtual int install(const Models::ProjectDescriptor &target) override {
    Utils::Unused{target};
    throw Core::Exceptions::NotImplementedException();
    return 0;
  }

  virtual int install(const Models::TargetDescriptor &target) override {
    Utils::Unused{target};
    throw Core::Exceptions::NotImplementedException();
    return 0;
  }
};

} // namespace CXPM::Models