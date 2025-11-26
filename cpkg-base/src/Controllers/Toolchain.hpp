#pragma once

#include <Core/Containers/Variant.hpp>
#include <Core/Logging/LoggerManager.hpp>

#include <Models/BasicProject.hpp>
#include <Models/BasicToolchain.hpp>
#include <Models/Commands/ArchiveCommand.hpp>
#include <Models/Commands/ExecutableCommand.hpp>
#include <Models/Commands/ObjectCommand.hpp>
#include <Models/Commands/SharedObjectCommand.hpp>

#include <Utils/Unix/EnvironmentManager.hpp>
#include <Utils/Unix/ShellManager.hpp>

#include <deque>
#include <dlfcn.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <utility>

using namespace Core::Containers;

namespace Controllers {

  using namespace Models;

class Toolchain;

namespace {
static inline std::deque<std::shared_ptr<Toolchain>> toolchains;
static inline std::shared_ptr<Toolchain> current_toolchain = nullptr;
} // namespace

class Toolchain : public BasicToolchain {
public:
  using BasicToolchain::BasicToolchain;

  template <typename... Args>
  Toolchain(Args... args) : BasicToolchain(std::forward<Args>(args)...) {}

  bool valid() {

    if (std::get<Core::Containers::String>(name).empty()) {
      return false;
    }

    if (Utils::Unix::EnvironmentManager::which(
            std::get<Core::Containers::String>(compiler_executable)) == "") {
      return false;
    }

    if (Utils::Unix::EnvironmentManager::which(
            std::get<Core::Containers::String>(linker_executable)) == "") {
      return false;
    }

    for (auto _dir : link_directories) {
      std::string directory = std::get<Core::Containers::String>(_dir);
      if (!std::filesystem::exists(directory)) {
        return false;
      }
    }

    for (auto _dir : include_directories) {
      std::string directory = std::get<Core::Containers::String>(_dir);
      if (!std::filesystem::exists(directory)) {
        return false;
      }
    }

    return true;
  }

  int build(std::shared_ptr<BasicProject> project) { return build(*project); }

  int build(const BasicProject &project) {
    for (auto package : project.targets) {
      if (build(std::get<BasicTarget>(package)) != 0) {
        return -1;
      }
    }

    return 0;
  }

  int build(const BasicTarget &package) {

    using promise_type =
        std::shared_ptr<std::promise<std::tuple<int, String, String>>>;

    std::deque<std::tuple<std::string, promise_type>> promises;

    for (auto source : package.sources) {

      auto command =
          Models::Commands::ObjectCommand::from(
              std::get<Core::Containers::String>(source), package, *this)
              .to_string();

      Core::Logging::LoggerManager::info("{}", command);
      auto promise = Utils::Unix::ShellManager::exec_async(command);
      promises.push_back({command, promise});
    }

    for (auto _promise : promises) {

      try {
        auto [command, promise] = _promise;

        auto future = promise->get_future();

        auto [retval, out, err] = future.get();

        if (!out.empty()) {
          Core::Logging::LoggerManager::error("{}", out);
        }

      } catch (std::exception &ex) {
        return -1;
      }
    }

    if (std::get<Core::Containers::String>(package.type) == "executable") {

      auto command =
          Models::Commands::ExecutableCommand::from(package, *this).to_string();
      auto [retval, out, err] = Utils::Unix::ShellManager::exec(command);
      if (retval != 0) {
        Core::Logging::LoggerManager::error("{}", out);
        return -1;
      }

      return 0;
    }

    if (std::get<Core::Containers::String>(package.type) == "shared-library") {

      auto command = Models::Commands::SharedObjectCommand::from(package, *this)
                         .to_string();
      auto [retval, out, err] = Utils::Unix::ShellManager::exec(command);
      if (retval != 0) {
        Core::Logging::LoggerManager::error("{}", out);
        return -1;
      }

      return 0;
    }

    if (std::get<Core::Containers::String>(package.type) == "static-library") {

      auto command =
          Models::Commands::ArchiveCommand::from(package, *this).to_string();
      auto [retval, out, err] = Utils::Unix::ShellManager::exec(command);
      if (retval != 0) {
        Core::Logging::LoggerManager::error("{}", out);
        return -1;
      }

      return 0;
    }

    if (std::get<Core::Containers::String>(package.type) == "object-library") {

      auto command =
          Models::Commands::ObjectCommand::from(package, *this).to_string();

      auto [retval, out, err] = Utils::Unix::ShellManager::exec(command);
      if (retval != 0) {
        Core::Logging::LoggerManager::error("{}", out);
        return -1;
      }

      return 0;
    }

    return -1;
  }

  int install(const std::shared_ptr<BasicProject> project) const { return 0; }

  int install(const BasicProject &project) {

    for (auto target : project.targets) {

      if (std::get<Core::Containers::String>(
              std::get<BasicTarget>(target).type) == "shared-library") {

        std::string name = std::get<Core::Containers::String>(
            std::get<BasicTarget>(target).name);

        std::string prefix = std::get<Core::Containers::String>(install_prefix);
        // Install the SO
        std::filesystem::copy(std::filesystem::path(name).append(".so"),
                              std::filesystem::path(prefix).append("/lib"));

        using directory_iterator =
            std::filesystem::recursive_directory_iterator;

        for (auto _include_directory :
             std::get<BasicTarget>(target).include_directories) {

          std::string include_directory =
              std::get<Core::Containers::String>(_include_directory);

          /// RATIONALE: (08:50 26/11/2025)
          /// failed to install, cannot install files outside project
          /// directory! this restriction is made to avoid confusion with
          /// include_directories you can set your includes based on how you do
          /// stuff, we consider headers in include directories within project
          /// as installable to avoid unnecessary customizations that other
          /// build systems do!

          // if (!include_directory.starts_with(target.project_directory())) {
          //   /// TODO: warn ignoring include
          //   continue;
          // }

          for (auto _file : directory_iterator(include_directory)) {

            auto file = _file;

            /// RATIONALE: (08:55 26/11/2025)
            /// uniform prefix is easy to form and search, when
            /// dealing with dependencies, dont' look for headers other than
            /// <prefix>/include/<pn>
            if (file.path().filename().string().ends_with("h")) {

              std::string name = std::get<Core::Containers::String>(
                  std::get<BasicTarget>(target).name);
              std::string prefix =
                  std::get<Core::Containers::String>(install_prefix);

              std::filesystem::copy(file, std::filesystem::path(prefix)
                                              .append("/include")
                                              .append(name));
            }

            if (file.path().filename().string().ends_with("hpp")) {

              std::string name = std::get<Core::Containers::String>(
                  std::get<BasicTarget>(target).name);
              std::string prefix =
                  std::get<Core::Containers::String>(install_prefix);

              std::filesystem::copy(file, std::filesystem::path(prefix)
                                              .append("/include")
                                              .append(name));
            }
          }
        }
      }
    }
    return 0;
  }

  int uninstall(const BasicProject &project) { return 0; }

  static inline constexpr const std::shared_ptr<Toolchain> current() {

    if (toolchains.empty()) {
      autoscan(Utils::Unix::EnvironmentManager::get(
          "CPKG_BUILD_EXTRA_MODULES_PATH"));
    }

    if (current_toolchain == nullptr) {
      current_toolchain = toolchains.front();
    }

    return current_toolchain;
  }

  static inline constexpr const void
  autoscan(std::deque<std::string> extra_paths = {}) {

    std::deque<std::string> search_paths = {
        "/usr/share/cpkg-build/toolchains",
        "/usr/local/share/cpkg-build/toolchains"};

    search_paths.append_range(extra_paths);

    toolchains.clear();

    for (auto path : search_paths) {
      using recursive_directory_iterator =
          std::filesystem::recursive_directory_iterator;
      if (!std::filesystem::exists(path) ||
          !std::filesystem::is_directory(path)) {
        continue;
      }
      for (auto plugin : recursive_directory_iterator(path)) {
        auto filename = std::filesystem::path(plugin).filename();

        if (filename.string().ends_with(".so")) {

          typedef Toolchain *(*getter_type)();

          void *handle = dlopen(std::filesystem::path(plugin).c_str(),
                                RTLD_LAZY | RTLD_DEEPBIND);

          try {

            auto get_toolchain =
                reinterpret_cast<getter_type>(dlsym(handle, "get_toolchain"));

            if (get_toolchain != nullptr) {

              auto toolchain = *get_toolchain();

              if (toolchain.valid()) {
                toolchains.push_back(std::make_shared<Toolchain>(toolchain));
              }
            }

          } catch (std::exception &ex) {
            // ignore and move out
          }

          if (handle != nullptr) {
            dlclose(handle);
          }
        }
      }
    }
  }
};
} // namespace Controllers