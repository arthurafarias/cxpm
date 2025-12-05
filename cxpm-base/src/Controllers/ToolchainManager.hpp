#pragma once

#include "Core/Logging/LoggerManager.hpp"
#include "Models/TargetDescriptor.hpp"
#include "Models/ToolchainDescriptor.hpp"

#include "Core/Exceptions/RuntimeException.hpp"
#include "Models/Toolchain.hpp"
#include "Utils/Unix/EnvironmentManager.hpp"

#include <dlfcn.h>
#include <filesystem>

using namespace Models;

namespace Controllers {
class ToolchainManager final {
  StaticClass(ToolchainManager)
public:
  static inline bool valid(const ToolchainDescriptor &toolchain) {

    if (toolchain.name.empty()) {
      return false;
    }

    if (Utils::Unix::EnvironmentManager::which(toolchain.compiler_executable) ==
        "") {
      return false;
    }

    if (Utils::Unix::EnvironmentManager::which(toolchain.linker_executable) ==
        "") {
      return false;
    }

    for (auto directory : toolchain.link_directories) {
      if (!std::filesystem::exists(directory.c_str())) {
        return false;
      }
    }

    for (auto directory : toolchain.include_directories) {
      if (!std::filesystem::exists(directory.c_str())) {
        return false;
      }
    }

    return true;
  }

  static inline constexpr const ToolchainDescriptor
  by_name(const String &name) {

    auto result = std::find_if(
        toolchains.begin(), toolchains.end(),
        [&name](auto toolchain) { return name == toolchain.name; });

    if (result == toolchains.end()) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't find toolchain with name {}", name);
    }

    return *result;
  };

  static inline constexpr const Toolchain
  autoselect(const TargetDescriptor &target) {
    auto result =
        std::find_if(toolchains.begin(), toolchains.end(),
                     [&target](const ToolchainDescriptor &toolchain) {
                       return target.language == toolchain.language;
                     });

    if (result == toolchains.end()) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't find a compatible toolchain to target {} with language {}",
          target.name, target.language);
    }

    return *result;
  };

  static inline constexpr void
  add(const ToolchainDescriptor &toolchain) {
    toolchains.push_back(toolchain);
  }

  static inline constexpr Toolchain current(const Collection<String>& extra_modules_paths) {

    if (current_toolchain != toolchains.end()) {
      autoscan(extra_modules_paths);
      current_toolchain = toolchains.begin();
    }

    if (current_toolchain == toolchains.end()) {
      throw Core::Exceptions::RuntimeException(
          "Failed to find any toolchain in this system");
    }

    return *current_toolchain;
  }

  static inline constexpr void
  autoscan(Collection<String> extra_paths = {}) {

    Collection<String> search_paths = {
        "/usr/share/cpkg/toolchains",
        "/usr/local/share/cpkg/toolchains"};

    search_paths.append_range(extra_paths);

    toolchains.clear();

    for (auto path : search_paths) {
      using recursive_directory_iterator =
          std::filesystem::recursive_directory_iterator;
      if (!std::filesystem::exists(path.c_str()) ||
          !std::filesystem::is_directory(path.c_str())) {
        continue;
      }
      for (auto plugin : recursive_directory_iterator(path.c_str())) {
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

              if (ToolchainManager::valid(toolchain)) {
                toolchains.push_back(toolchain);
              }
            }

          } catch (std::exception &ex) {
            Core::Logging::LoggerManager::error("{}", ex.what());
          }

          if (handle != nullptr) {
            dlclose(handle);
          }
        }
      }
    }
  }

private:
  static inline Collection<ToolchainDescriptor> toolchains;
  static inline Collection<ToolchainDescriptor>::iterator
      current_toolchain;
};
} // namespace Controllers