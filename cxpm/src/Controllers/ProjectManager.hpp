#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Models/CompilerCommandDescriptor.hpp"
#include "Models/Project.hpp"
#include "Models/ProjectDescriptor.hpp"
#include "Models/Target.hpp"
#include "Models/TargetDescriptor.hpp"
#include "Models/Toolchain.hpp"
#include "Models/ToolchainDescriptor.hpp"
#include "Modules/Serialization/JsonOutputArchiver.hpp"
#include <Controllers/ToolchainManager.hpp>
#include <Core/Containers/String.hpp>
#include <Utils/Unused.hpp>
#include <dlfcn.h>

#include <filesystem>
#include <fstream>
#include <tuple>

#ifndef cxpm_BASE_INSTALL_PREFIX
#define cxpm_BASE_INSTALL_PREFIX "/usr/local"
#endif

#ifndef cxpm_BASE_SOURCE_PREFIX
#define cxpm_BASE_SOURCE_PREFIX "/usr/src/cxpm/cxpm-base"
#endif

#ifndef cxpm_BUILD_INSTALL_PREFIX
#define cxpm_BUILD_INSTALL_PREFIX "/usr"
#endif

namespace Controllers {

class ProjectManager final {

StaticClass(ProjectManager)

    public :

    enum class BuildProjectOutputResultStatus {
      Success,
      Failure
    };

  static inline void initialize() {
    modules_search_paths =
        Utils::Unix::EnvironmentManager::get("cxpm_BUILD_EXTRA_MODULES_PATH");

    extra_module_path_add("/usr/share/cxpm/toolchains");
    extra_module_path_add("/usr/local/share/cxpm/toolchains");

    extra_module_path_add("/usr/lib/cxpm/toolchains");
    extra_module_path_add("/usr/local/lib/cxpm/toolchains");

    extra_module_path_add(cxpm_BASE_INSTALL_PREFIX "/lib/cxpm/toolchains");
    extra_module_path_add(cxpm_BASE_INSTALL_PREFIX "/share/cxpm/toolchains");
  }

  using BuildProjectOutputResult =
      std::tuple<BuildProjectOutputResultStatus, ProjectDescriptor,
                 ToolchainDescriptor>;

  static void extra_module_path_add(const char *try_path) {
    if (std::filesystem::exists(try_path) &&
        std::filesystem::is_directory(try_path)) {
      using directory_iterator = std::filesystem::directory_iterator;
      for (auto dir : directory_iterator(try_path)) {
        if (dir.is_directory()) {
          modules_search_paths.push_back(dir.path().string());
        }
      }
    }
  }

  static inline Collection<String> modules_search_paths;

  static inline BuildProjectOutputResult build_project(const String directory) {

    Toolchain toolchain;
    Collection<CompileCommandDescriptor> commands;

    auto build_path = std::filesystem::path(directory.c_str());

    if (!directory.empty()) {
      build_path = directory.c_str();
    }

    auto [build_manifest_result, build_manifest_comands] =
        Controllers::ProjectManager::build_manifest(build_path.string(),
                                                    modules_search_paths);

    commands.append_range(build_manifest_comands);

    if (build_manifest_result != 0) {

      throw Core::Exceptions::RuntimeException(
          "Failed to build manifest {} using {}",
          build_path.string() + "/project-manifest.so",
          build_path.string() + "/package.json");
    }

    auto project_manifest = Controllers::ProjectManager::load_from_manifest(
        std::filesystem::path(build_path.string())
            .append("libproject-manifest.so")
            .string());

    project_manifest.compile_comands = build_manifest_comands;

    for (auto toolchain : project_manifest.toolchains) {
      try {

        if (Controllers::ToolchainManager::valid(toolchain)) {
          Controllers::ToolchainManager::add(toolchain);
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error(
            "Couldn't append toolchain specified in project {}", ex.what());
      }
    }

    for (auto target : project_manifest.targets) {
      try {

        toolchain = Controllers::ToolchainManager::autoselect(target);

        if (target.toolchain != "") {
          toolchain = Controllers::ToolchainManager::by_name(target.toolchain);
        }

        auto [build_result, build_commands] = toolchain.build(target);

        target.compile_commands = build_commands;

        commands.append_range(target.compile_commands);

        if (build_result != 0) {
          throw Core::Exceptions::RuntimeException(
              "Couldn't build project {} with language {} and toolchain {}",
              target.name, target.language, toolchain.name);
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error("Couldn't build the project: {}",
                                            ex.what());
      }
    }

    auto stream = std::ofstream("compile_commands.json");
    Modules::Serialization::JsonOutputArchiver output(stream);
    output % commands;

    return {BuildProjectOutputResultStatus::Success, project_manifest,
            toolchain};
  }

  enum class BuildTargetOutputResultStatus { Success, Failure };
  using BuildTargetOutputREsult =
      std::tuple<BuildTargetOutputResultStatus, TargetDescriptor,
                 ToolchainDescriptor>;

  static inline BuildTargetOutputREsult
  build_target(const TargetDescriptor &target, const String &prefix_override) {
    Toolchain toolchain;
    return {BuildTargetOutputResultStatus::Success, target, toolchain};
  }

  enum class InstallPRojectOutputResultStatus { Success, Failure };

  using InstallProjectOutputResult =
      std::tuple<InstallPRojectOutputResultStatus>;

  static inline InstallProjectOutputResult
  install_project(const Project &target, const String prefix) {
    return {InstallPRojectOutputResultStatus::Success};
  }
  enum class InstallTargetOutputResultStatus { Success, Failure };

  using InstallTargetOutputResult =
      std::tuple<InstallTargetOutputResultStatus, TargetDescriptor,
                 ToolchainDescriptor>;

  static inline InstallTargetOutputResult
  install_target(TargetDescriptor &target, const ToolchainDescriptor &toolchain,
                 String prefix_override = "") {
    // include directory should be in project directory
    using directory_iterator = std::filesystem::directory_iterator;

    String prefix = toolchain.install_prefix;

    if (!prefix_override.empty()) {
      prefix = prefix_override;
    }

    // install include directories
    for (auto directory : target.include_directories) {

      auto header_install_path = std::filesystem::path()
                                     .append(prefix.c_str())
                                     .append("include")
                                     .append(target.name.c_str());

      auto binnaries_install_path =
          std::filesystem::path().append(prefix.c_str()).append("bin");

      auto archive_install_path =
          std::filesystem::path().append(prefix.c_str()).append("lib");

      auto library_install_path =
          std::filesystem::path().append(prefix.c_str()).append("lib");

      auto pc_install_path = std::filesystem::path()
                                 .append(prefix.c_str())
                                 .append("lib")
                                 .append("pkgconfig");

      if (!std::filesystem::is_directory(header_install_path)) {
        std::filesystem::create_directories(header_install_path);
      }

      if (!std::filesystem::is_directory(binnaries_install_path)) {
        std::filesystem::create_directories(binnaries_install_path);
      }

      if (!std::filesystem::is_directory(library_install_path)) {
        std::filesystem::create_directories(library_install_path);
      }

      if (!std::filesystem::is_directory(archive_install_path)) {
        std::filesystem::create_directories(archive_install_path);
      }

      if (!std::filesystem::is_directory(pc_install_path)) {
        std::filesystem::create_directories(pc_install_path);
      }

      for (auto file : directory_iterator(directory.c_str())) {

        if (!(file.path().extension() == ".h") &&
            !(file.path().extension() == ".hpp") &&
            !(file.path().extension() == ".hxx") &&
            !(file.path().extension() == ".hh")) {
          continue;
        }

        std::filesystem::copy(
            file, header_install_path,
            std::filesystem::copy_options::recursive |
                std::filesystem::copy_options::overwrite_existing);
      }

      if (target.type == "executable") {
        std::filesystem::copy(
            target.name.c_str(), std::filesystem::path(binnaries_install_path),
            std::filesystem::copy_options::overwrite_existing);
      }

      if (target.type == "shared-library") {
        std::filesystem::copy(
            "lib" + target.name + ".so",
            std::filesystem::path(library_install_path),
            std::filesystem::copy_options::overwrite_existing);
      }

      if (target.type == "static-library") {
        std::filesystem::copy(
            "lib" + target.name + ".a",
            std::filesystem::path(archive_install_path),
            std::filesystem::copy_options::overwrite_existing);
      }

      if (target.type == "object-library") {
        std::filesystem::copy(
            target.name + ".o", std::filesystem::path(library_install_path),
            std::filesystem::copy_options::overwrite_existing);
      }

      // install pc file
      {
        using namespace Models;

        auto rendered = std::format(
            "Name: {}\n"
            "Description: {}\n"
            "Version: {}\n"
            "URL: {}\n"
            "Cflags: -I{}/include/{}\n"
            "Libs: -l{}\n",
            target.name, target.description, target.version, target.url,
            target.install_prefix, target.name, target.name);

        auto pc_file_stream =
            std::ofstream(pc_install_path.append(target.name + ".pc"),
                          std::ios_base::out | std::ios_base::trunc);
        auto syncstream = std::osyncstream(pc_file_stream);
        syncstream << rendered;
      }
    }

    return InstallTargetOutputResult{InstallTargetOutputResultStatus::Success,
                                     target, toolchain};
  }

  enum class UninstallTargetOutputResultStatus { Success, Failure };

  using UninstallTargetOutputResult =
      std::tuple<UninstallTargetOutputResultStatus, TargetDescriptor>;

  static inline UninstallTargetOutputResult
  uninstall_target(const TargetDescriptor &target, const String &prefix) {
    Utils::Unused{prefix};
    return {UninstallTargetOutputResultStatus::Success, target};
  }

  enum class UninstallProjectOutputResultStatus { Success, Failure };

  using UninstallProjectOutputResult =
      std::tuple<UninstallProjectOutputResultStatus, ProjectDescriptor>;

  static inline UninstallProjectOutputResult
  uninstall_project(const ProjectDescriptor &project, const String &) {

    return {UninstallProjectOutputResultStatus::Success, project};
  }

  enum BuildManifestResultStatus { Success, Failure };

  using BuildManifestResult = std::tuple<BuildManifestResultStatus,
                                         Collection<CompileCommandDescriptor>>;

  static inline BuildManifestResult
  build_manifest(const Core::Containers::String &project_path,
                 Core::Containers::Collection<Core::Containers::String>
                     extra_toolchain_search_paths) {

    auto [generate_loader_result, loader_path] = generate_loader(project_path);

    if (generate_loader_result == GenerateLoaderResultStatus::Failure) {
      return BuildManifestResult(BuildManifestResultStatus::Failure,
                                 Collection<CompileCommandDescriptor>());
    }

    auto [loader_build_result, loader_compile_commands] =
        Toolchain(Controllers::ToolchainManager::current(
                      extra_toolchain_search_paths))
            .build(ManifestPackage);

    return {BuildManifestResultStatus::Success, loader_compile_commands};
  }

  static inline Models::ProjectDescriptor
  load_from_manifest(const Core::Containers::String &manifest_path) {

    typedef Models::Project *(*getter_type)();

    void *handle = dlopen(manifest_path.c_str(), RTLD_NOW | RTLD_DEEPBIND);

    if (handle == nullptr) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't load manifest at path {}: not found!", manifest_path);
    }

    auto get_project =
        reinterpret_cast<getter_type>(dlsym(handle, "get_project"));

    if (get_project == nullptr) {
      dlclose(handle);
      throw Core::Exceptions::RuntimeException(
          "Couldn't load manifest at path {}: malformed get_project method",
          manifest_path);
    }

    auto retval = *get_project();

    dlclose(handle);

    return retval;
  }

  static inline int clean(const String &project_path,
                          const Collection<String> &) {

    // objects
    for (auto source : ManifestPackage.sources) {
      std::filesystem::remove_all(std::filesystem::path(project_path.c_str())
                                      .append(source.c_str())
                                      .append(".o"));
    }

    // generate packages
    std::filesystem::remove_all(
        std::filesystem::path(project_path.c_str()).append("package.cpp.o"));

    std::filesystem::remove_all(std::filesystem::path(project_path.c_str())
                                    .append("package.loader.cpp.o"));

    std::filesystem::remove_all(std::filesystem::path(project_path.c_str())
                                    .append("package.loader.cpp"));

    return 0;
  }

  Core::Containers::Collection<Models::ProjectDescriptor> projects;

private:
  static inline Models::Target ManifestPackage =
      Models::Target()
          .name_set("project-manifest")
          .type_set("shared-library")
          .include_directories_append({
              String(cxpm_BASE_INSTALL_PREFIX) + "/lib/cxpm-base/headers",
              String(cxpm_BASE_INSTALL_PREFIX) + "/share/cxpm-base/headers",
              String(cxpm_BASE_INSTALL_PREFIX) + "/include/cxpm-base",
              String(cxpm_BASE_SOURCE_PREFIX) + "/src",
          })
          .options_append({"-std=c++23", "-Wall", "-Werror", "-pedantic"})
          .sources_append({"package.cpp", "package.loader.cpp"})
          .create();

  static inline const std::string BasicProjectLoaderSource = R"(
    #include <Models/ProjectDescriptor.hpp>
    using namespace Models;
    extern ProjectDescriptor project;
    // should be a weak reference that can be overriten by a custom get_project // more versatile but unsafe.
    extern "C" const ProjectDescriptor* get_project()  { return &project; }
  )";

  enum class GenerateLoaderResultStatus { Success, Failure };

  using GenerateLoaderResult = std::tuple<GenerateLoaderResultStatus, String>;

  static GenerateLoaderResult generate_loader(std::string base_path) {
    std::ofstream loader_source(base_path + "/package.loader.cpp",
                                std::ios_base::trunc | std::ios_base::out);
    loader_source << BasicProjectLoaderSource;
    loader_source.flush();
    return {
        GenerateLoaderResultStatus::Success,
        std::filesystem::path(base_path).append("package.loader.cpp").string()};
  }
};
} // namespace Controllers