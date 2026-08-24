#pragma once

#include "CXPM/Core/Logging/LoggerManager.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/ToolchainDescriptor.hpp"

#include "CXPM/Core/Exceptions/RuntimeException.hpp"
#include "CXPM/Utils/Macros/StaticClass.hpp"
#include "CXPM/Utils/Unix/EnvironmentManager.hpp"
#include "CXPM/Utils/Unix/ShellManager.hpp"

#include <algorithm>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>

using namespace CXPM;

#ifndef cxpm_BASE_INSTALL_PREFIX
#define cxpm_BASE_INSTALL_PREFIX "/usr/local"
#endif

#ifndef cxpm_BASE_SOURCE_PREFIX
#define cxpm_BASE_SOURCE_PREFIX "/usr/src/cxpm/applications/cxpm"
#endif

namespace CXPM::Controllers {
class ToolchainManager final {
StaticClass(ToolchainManager) public
    : static inline bool valid(const ToolchainDescriptor &toolchain) {

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

  static inline constexpr const ToolchainDescriptor
  autoselect(const TargetDescriptor &target) {
    auto result = std::find_if(toolchains.begin(), toolchains.end(),
                               [&target](const ToolchainDescriptor &toolchain) {
                                std::cout << std::format("trying to select toolchain {} with language {} for target {} with language {}\n", toolchain.name, toolchain.language, target.name, target.language);
                                 return target.language == toolchain.language;
                               });

    if (result == toolchains.end()) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't find a compatible toolchain to target {} with language {}",
          target.name, target.language);
    }

    std::cout << std::format("Selecting Toolchain: {} with language {} at {}\n", result->name, result->language, result->compiler_executable);

    return *result;
  };

  static inline constexpr void add(const ToolchainDescriptor &toolchain) {
    toolchains.push_back(toolchain);
  }

  static inline ToolchainDescriptor
  current(const String &project_path,
         const BasicCollection<String> &extra_modules_paths) {

    autoscan(project_path, extra_modules_paths);

    // the project manifest is always a c++ shared library, so it needs a
    // toolchain that can actually build c++, not just whichever one happened
    // to be discovered first
    auto result =
        std::find_if(toolchains.begin(), toolchains.end(),
                     [](const auto &toolchain) {
                       return toolchain.language == "c++";
                     });

    if (result == toolchains.end()) {
      throw Core::Exceptions::RuntimeException(
          "Failed to find a c++ toolchain in this system");
    }

    return *result;
  }

  static inline void
  autoscan(const String &project_path,
          BasicCollection<String> extra_paths = {}) {

    auto const HOME = Utils::Unix::EnvironmentManager::get("HOME").front();

    BasicCollection<String> search_paths = {
        "/usr/share/cxpm/toolchains",
        "/usr/local/share/cxpm/toolchains",
        std::filesystem::path()
            .append(HOME.c_str())
            .append(".local/lib")
            .c_str(),
        std::filesystem::path()
            .append(HOME.c_str())
            .append(".local/share/toolchains/cxpm")
            .c_str(),
        std::filesystem::path()
            .append(HOME.c_str())
            .append(".local/lib/toolchains/cxpm")
            .c_str(),

    };

    search_paths.append_range(extra_paths);

    toolchains.clear();

    for (auto path : search_paths) {
      using recursive_directory_iterator =
          std::filesystem::recursive_directory_iterator;
      if (!std::filesystem::exists(path.c_str()) ||
          !std::filesystem::is_directory(path.c_str())) {
        continue;
      }
      for (auto entry : recursive_directory_iterator(path.c_str())) {

        if (!entry.is_regular_file() ||
            entry.path().filename() != "toolchain.cpp") {
          continue;
        }

        try {

          auto descriptor = build_toolchain_plugin(entry.path(), project_path);

          if (ToolchainManager::valid(descriptor)) {
            toolchains.push_back(descriptor);
          }

        } catch (std::exception &ex) {
          Core::Logging::LoggerManager::error("{}", ex.what());
        }
      }
    }
  }

private:
  static inline const std::string ToolchainLoaderSource = R"(
    #include <CXPM/ToolchainDescriptor.hpp>
    using namespace CXPM;
    extern ToolchainDescriptor toolchain;
    extern "C" const ToolchainDescriptor* get_toolchain()  { return &toolchain; }
  )";

  static inline ToolchainDescriptor
  build_toolchain_plugin(const std::filesystem::path &source_path,
                         const String &project_path) {

    auto name = source_path.parent_path().filename().string();

    auto loader_path = std::filesystem::path(project_path.c_str())
                           .append("toolchain-" + name + ".loader.cpp");

    std::ofstream loader_stream(loader_path,
                                std::ios_base::trunc | std::ios_base::out);
    loader_stream << ToolchainLoaderSource;
    loader_stream.flush();

    auto library_path = std::filesystem::path(project_path.c_str())
                            .append("libtoolchain-" + name + ".so");

    auto compiler = Utils::Unix::EnvironmentManager::which("c++");

    if (compiler.empty()) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't find a c++ compiler to bootstrap toolchain {}", name);
    }

    auto command = std::format(
        "{} -std=c++23 -fPIC -shared -I{} -I{} -I{} -I{} {} {} -o {}",
        compiler.c_str(),
        (std::filesystem::path(cxpm_BASE_INSTALL_PREFIX) / "lib/cxpm/headers")
            .string(),
        (std::filesystem::path(cxpm_BASE_INSTALL_PREFIX) / "share/cxpm/headers")
            .string(),
        (std::filesystem::path(cxpm_BASE_INSTALL_PREFIX) / "include").string(),
        (std::filesystem::path(cxpm_BASE_SOURCE_PREFIX) / "src").string(),
        source_path.string(), loader_path.string(), library_path.string());

    auto build_result = Utils::Unix::ShellManager::exec(command);

    if (std::get<0>(build_result) != 0) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't build toolchain plugin {}: {}", name,
          std::get<2>(build_result));
    }

    return load_toolchain_plugin(library_path.string());
  }

  static inline ToolchainDescriptor
  load_toolchain_plugin(const String &library_path) {

    typedef const ToolchainDescriptor *(*getter_type)();

    void *handle = dlopen(library_path.c_str(), RTLD_NOW | RTLD_DEEPBIND);

    if (handle == nullptr) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't load toolchain plugin at path {}: not found!",
          library_path);
    }

    auto get_toolchain =
        reinterpret_cast<getter_type>(dlsym(handle, "get_toolchain"));

    if (get_toolchain == nullptr) {
      dlclose(handle);
      throw Core::Exceptions::RuntimeException(
          "Couldn't load toolchain plugin at path {}: malformed get_toolchain "
          "method",
          library_path);
    }

    auto descriptor = *get_toolchain();

    dlclose(handle);

    return descriptor;
  }

  static inline BasicCollection<ToolchainDescriptor> toolchains;
};
} // namespace CXPM::Controllers
