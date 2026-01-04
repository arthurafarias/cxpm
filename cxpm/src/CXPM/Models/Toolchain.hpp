#pragma once

#include "CXPM/Models/PackageDescriptor.hpp"
#include "CXPM/Models/Resources/SharedObjectSuffix.hpp"
#include "CXPM/Models/TargetDescriptor.hpp"
#include <CXPM/Controllers/PkgConfigManager.hpp>
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Core/Containers/Variant.hpp>
#include <CXPM/Core/Exceptions/NotImplementedException.hpp>
#include <CXPM/Models/BuildOutputResult.hpp>
#include <CXPM/Models/CompilerCommandDescriptor.hpp>
#include <CXPM/Models/ProjectDescriptor.hpp>
#include <CXPM/Models/Toolchain.hpp>
#include <CXPM/Models/ToolchainBuildInterface.hpp>
#include <CXPM/Models/ToolchainDescriptor.hpp>
#include <CXPM/Models/ToolchainInstallInterface.hpp>
#include <CXPM/Modules/Logging/LoggerManager.hpp>
#include <CXPM/Utils/Unix/ShellManager.hpp>
#include <CXPM/Utils/Unused.hpp>

#include <algorithm>
#include <filesystem>
#include <format>
#include <future>
#include <optional>
#include <ranges>
#include <type_traits>

#define ExportToolchain(name)                                                  \
  extern "C" Toolchain *get_toolchain() { return &name; }

using namespace CXPM::Core::Containers;

namespace CXPM::Models {

class Toolchain : public ToolchainDescriptor,
                  public ToolchainBuildInterface,
                  public ToolchainInstallInterface {

public:
  Toolchain() : ToolchainDescriptor() {}
  Toolchain(const ToolchainDescriptor &descriptor)
      : ToolchainDescriptor(descriptor) {}

  Toolchain &name_set(const String &value) {
    name = value;
    return *this;
  };

  const String &name_get() { return name; };

  Toolchain &version_set(const String &value) {
    version = value;
    return *this;
  };

  const String &version_get() { return version; };

  Toolchain &language_set(const String &value) {
    language = value;
    return *this;
  };

  const String &language_get() { return language; };

  Toolchain &install_prefix_set(const String &value) {
    install_prefix = value;
    return *this;
  };

  const String &install_prefix_get() { return install_prefix; };

  Toolchain &include_directory_prefix_set(const String &value) {
    include_directory_prefix = value;
    return *this;
  };

  const String &include_directory_prefix_get() {
    return include_directory_prefix;
  };

  Toolchain &include_directories_set(const Collection<String> &value) {
    include_directories = value;
    return *this;
  };

  const Collection<String> &include_directories_get() {
    return include_directories;
  };

  Toolchain &compiler_executable_set(const String &value) {
    compiler_executable = value;
    return *this;
  };

  const String &compiler_executable_get() { return compiler_executable; };

  Toolchain &compiler_options_set(const Collection<String> &value) {
    compiler_options = value;
    return *this;
  };

  Toolchain &compiler_options_append(const String &value) {
    compiler_options.push_back(value);
    return *this;
  };

  const Collection<String> &compiler_options_get() { return compiler_options; };

  Toolchain &archiver_executable_set(const String &value) {
    archiver_executable = value;
    return *this;
  };

  Toolchain &executable_options_set(const Collection<String> &value) {
    executable_options = value;
    return *this;
  };

  const String &archiver_executable_get() { return archiver_executable; };

  Toolchain &archiver_options_set(const Collection<String> &value) {
    archiver_options = value;
    return *this;
  };

  Toolchain &archiver_options_append(const String &value) {
    archiver_options.push_back(value);
    return *this;
  };

  const Collection<String> &archiver_options_get() { return archiver_options; };

  Toolchain &linker_executable_set(const String &value) {
    linker_executable = value;
    return *this;
  };

  const String &linker_executable_get() { return linker_executable; };

  Toolchain &linker_options_set(const Collection<String> &value) {
    linker_options = value;
    return *this;
  };

  const Collection<String> &linker_options_get() { return linker_options; };

  Toolchain &link_directory_prefix_set(const String &value) {
    link_directory_prefix = value;
    return *this;
  };

  const String &link_directory_prefix_get() { return link_directory_prefix; };

  Toolchain &link_library_prefix_set(const String &value) {
    link_library_prefix = value;
    return *this;
  };

  const String &link_library_prefix_get() { return link_library_prefix; };

  Toolchain &source_specifier_prefix_set(const String &value) {
    source_specifier_prefix = value;
    return *this;
  };

  const String &source_specifier_prefix_get() {
    return source_specifier_prefix;
  };

  Toolchain &object_specifier_prefix_set(const String &value) {
    object_specifier_prefix = value;
    return *this;
  };

  const String &object_specifier_prefix_get() {
    return object_specifier_prefix;
  };

  Toolchain &link_directories_set(const Collection<String> &value) {
    link_directories = value;
    return *this;
  };

  Toolchain &link_directories_append(const String &value) {
    link_directories.push_back(value);
    return *this;
  };

  const Collection<String> &link_directories_get() { return link_directories; };

  virtual ObjectBuildResult object_build(const String &source,
                                         const TargetDescriptor &target,
                                         bool dry = false) override {
    Modules::Logging::LoggerManager::info("building {}: started", source);

    Collection<String> command;

    command.push_back(compiler_executable);

    command.append_range(target.options);

    command.push_back("-fPIC");

    auto include_directories =
        target.include_directories |
        std::views::transform([&](auto &&directory) {
          auto directory_path = std::filesystem::path(directory.c_str());

          if (directory_path.is_relative()) {
            directory_path = std::filesystem::path(target.build_path.c_str())
                                 .append(directory_path.c_str());
          }

          return std::format("{} {}", include_directory_prefix,
                             directory_path.string());
        });

    command.append_range(include_directories);

    auto link_directories =
        target.link_directories | std::views::transform([&](auto &&directory) {
          auto directory_path = std::filesystem::path(directory.c_str());

          if (directory_path.is_relative()) {
            directory_path = std::filesystem::path(target.build_path.c_str())
                                 .append(directory_path.c_str());
          }

          return std::format("{} {}", link_directory_prefix,
                             directory_path.string());
        });

    command.append_range(link_directories);

    auto source_path =
        std::filesystem::path(target.build_path.c_str()).append(source.c_str());

    auto object_path = source_path;
    object_path.concat(".o");

    command.push_back(source_specifier_prefix);
    command.push_back(source_path.c_str());

    command.push_back(object_specifier_prefix);
    command.push_back(object_path.c_str());

    auto command_line = String::join(command, " ");

    auto status = ObjectBuildResultStatus::Success;

    const auto &[result_code, out, err] =
        Utils::Unix::ShellManager::exec(command_line, dry);

    if (result_code != 0) {
      status = ObjectBuildResultStatus::Failure;
    }

    Modules::Logging::LoggerManager::info("building {}: ended", source);

    return {status, CompileCommandDescriptor{
                        .directory = std::filesystem::current_path(),
                        .command = command_line,
                        .file = source_path.c_str(),
                        .output = object_path.c_str(),
                        .stdout = out,
                        .stderr = err,
                    }};
  }

  ObjectBuildResultPromiseType
  object_build_async(const String &source, const TargetDescriptor &target,
                     bool dry) override {
    return std::async(std::launch::async,
                      [this, source, target, dry]() {
                        return this->object_build(source, target, dry);
                      })
        .share();
  }

  Set<String> command_link_libraries_prepare(const Target &target) {
    return target.link_libraries | std::views::transform([&](auto &&library) {
             return std::format("{} {}", link_library_prefix, library);
           }) |
           std::ranges::to<Set<String>>();
  }

  Set<String> command_link_directories_prepare(const Target &target) {
    auto result =
        target.link_directories | std::views::transform([&](auto &&directory) {
          auto directory_path = std::filesystem::path(directory.c_str());

          if (directory_path.is_relative()) {
            directory_path = std::filesystem::path(target.build_path.c_str())
                                 .append(directory.c_str());
          }

          return std::format("{} {}", link_directory_prefix,
                             directory_path.c_str());
        }) |
        std::ranges::to<Set<String>>();
    return result;
  }

  Set<String> command_include_directories_prepare(const Target &target) {
    auto result =
        target.include_directories |
        std::views::transform([&](auto &&directory) {
          auto directory_path = std::filesystem::path(directory.c_str());

          if (directory_path.is_relative()) {
            directory_path = std::filesystem::path(target.build_path.c_str())
                                 .append(directory.c_str());
          }

          return std::format("{} {}", include_directory_prefix,
                             directory_path.c_str());
        }) |
        std::ranges::to<Set<String>>();

    return result;
  }

  Set<String> command_objects_prepare(const Target &target) {
    return target.sources | std::views::transform([&](auto &&source) {
             auto source_path = std::filesystem::path(source.c_str());
             if (source_path.is_relative()) {
               source_path = std::filesystem::path(target.build_path.c_str())
                                 .append(source.c_str());
             }
             return std::format("{}{}", source_path.c_str(), ".o");
           }) |
           std::ranges::to<Set<String>>();
  }

  virtual ExecutableLinkResult executable_link(const TargetDescriptor &input,
                                               bool dry = false) override {
    Target target = input;
    Collection<String> command;

    command.push_back(compiler_executable);

    command.append_range(target.options);

    command.append_range(executable_options);

    command.append_range(command_include_directories_prepare(target));
    command.append_range(command_link_directories_prepare(target));
    command.append_range(command_link_libraries_prepare(target));

    command.push_back(
        std::format("{}{}", object_specifier_prefix, target.name));

    command.append_range(command_objects_prepare(target));

    auto command_line = String::join(command, " ");

    const auto &[result_code, out, err] =
        Utils::Unix::ShellManager::exec(command_line, dry);

    ExecutableLinkResultStatus status = ExecutableLinkResultStatus::Success;

    if (result_code != 0) {
      status = ExecutableLinkResultStatus::Failure;
    }

    Modules::Logging::LoggerManager::info("building: ended");

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
    return std::async(std::launch::async,
                      [this, target]() { return executable_link(target); })
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

    Collection<String> command;

    command.push_back(compiler_executable);

    command.push_back("-fPIC");
    command.push_back("-shared");

    command.append_range(this->linker_options);

    command.append_range(target.options);
    command.append_range(command_include_directories_prepare(target));
    command.append_range(command_link_directories_prepare(target));
    command.append_range(command_link_libraries_prepare(target));

    auto output_path = std::filesystem::path(target.build_path.c_str())
                           .append(library_prefix + target.name +
                                   Resources::SharedObjectSuffix());

    command.append_range(Collection<String>{"-o", output_path.c_str()});

    command.append_range(command_objects_prepare(target));

    auto command_line = String::join(command, " ");

    const auto &[result_code, out, err] =
        Utils::Unix::ShellManager::exec(command_line, dry);

    SharedObjectLinkResultStatus status = SharedObjectLinkResultStatus::Success;

    if (result_code != 0) {
      status = SharedObjectLinkResultStatus::Failure;
    }

    Modules::Logging::LoggerManager::info("building: ended");

    return {status, CompileCommandDescriptor{
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
    return std::async(std::launch::async,
                      [this, target, dry, library_prefix]() {
                        return archive_link(target, dry, library_prefix);
                      })
        .share();
  }

  virtual ArchiveLinkResult
  archive_link(const TargetDescriptor &target, bool dry = false,
               const String &library_prefix = "lib") override {

    Collection<String> command;

    command.push_back(archiver_executable);

    command.append_range(this->archiver_options);
    command.append_range(command_include_directories_prepare(target));
    command.append_range(command_link_directories_prepare(target));
    command.append_range(command_link_libraries_prepare(target));

    command.append_range(
        Collection<String>{"-o", library_prefix + target.name + ".a"});

    command.append_range(command_objects_prepare(target));

    auto command_line = String::join(command, " ");

    const auto &[result_code, out, err] =
        Utils::Unix::ShellManager::exec(command_line, dry);

    auto status = ArchiverLinkResultStatus::Success;

    if (result_code != 0) {
      status = ArchiverLinkResultStatus::Failure;
    }

    Modules::Logging::LoggerManager::info("building: ended");

    return {status, CompileCommandDescriptor{
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
      if (code != 0) {
        break;
      }
    }

    return retval;
  }

  virtual BuildOutputResult build(const TargetDescriptor &input,
                                  bool dry = false) override {

    auto target = input;

    auto result_commands = Collection<CompileCommandDescriptor>();    

    std::deque<ObjectBuildResultPromiseType> results;

    for (auto source : target.sources) {
      results.push_back(object_build_async(source, target, dry));
    };

    for (auto result : results) {

      auto [code, commands] = result.get();

      Modules::Logging::LoggerManager::debug("{}", std::to_string(code));

      result_commands.push_back(commands);

      if (code != ObjectBuildResultStatus::Success) {
        return {BuildOutputResultStatus::Failure, result_commands};
      }
    }

    if (target.type == "executable") {

      auto [code, commands] = executable_link(target, dry);

      Modules::Logging::LoggerManager::debug("{}", std::to_string(code));
      result_commands.push_back(commands);

      if (code != ExecutableLinkResultStatus::Success) {
        return {BuildOutputResultStatus::Failure, result_commands};
      }
    }

    if (target.type == "shared-library") {

      auto [code, commands] = shared_object_link(target);

      Modules::Logging::LoggerManager::debug("{}", std::to_string(code));

      result_commands.push_back(commands);

      if (code != SharedObjectLinkResultStatus::Success) {
        return {BuildOutputResultStatus::Failure, result_commands};
      }
    }

    if (target.type == "static-library") {

      auto [code, commands] = archive_link(target, dry);

      Modules::Logging::LoggerManager::debug("{}", std::to_string(code));

      result_commands.push_back(commands);

      if (code != ArchiverLinkResultStatus::Success) {
        return {BuildOutputResultStatus::Failure, result_commands};
      }
    }

    return {BuildOutputResultStatus::Success, result_commands};
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

  Toolchain &create() { return *this; }
};

} // namespace CXPM::Models
