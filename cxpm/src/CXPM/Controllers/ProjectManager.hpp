#pragma once

#include "CXPM/Core/Containers/Map.hpp"
#include "CXPM/Core/Containers/Set.hpp"
#include "CXPM/Models/BuildOutputResult.hpp"
#include "CXPM/Modules/Logging/LoggerManager.hpp"
#include "CXPM/Utils/Unix/EnvironmentManager.hpp"
#include <CXPM/Controllers/ToolchainManager.hpp>
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Core/Exceptions/RuntimeException.hpp>
#include <CXPM/Models/CompilerCommandDescriptor.hpp>
#include <CXPM/Models/Project.hpp>
#include <CXPM/Models/ProjectDescriptor.hpp>
#include <CXPM/Models/Target.hpp>
#include <CXPM/Models/TargetDescriptor.hpp>
#include <CXPM/Models/Toolchain.hpp>
#include <CXPM/Models/ToolchainDescriptor.hpp>
#include <CXPM/Modules/Serialization/JsonOutputArchiver.hpp>
#include <CXPM/Utils/Unused.hpp>
#include <algorithm>
#include <dlfcn.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <ranges>
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

namespace CXPM::Controllers {

class ProjectManager final {

  StaticClass(ProjectManager);

public:
  enum class BuildProjectOutputResultStatus { Success, Failure };

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

  static inline BuildProjectOutputResult
  project_build(const ProjectDescriptor &project_manifest) {

    Toolchain toolchain;
    Collection<CompileCommandDescriptor> compile_commands;

    for (auto toolchain : project_manifest.toolchains) {
      try {

        if (Controllers::ToolchainManager::valid(toolchain)) {
          Controllers::ToolchainManager::add(toolchain);
        }

      } catch (std::exception &ex) {
        Modules::Logging::LoggerManager::error(
            "Couldn't append toolchain specified in project {}", ex.what());
      }
    }

    for (Target target : project_manifest.targets) {
      try {

        toolchain = Controllers::ToolchainManager::autoselect(target);

        if (target.toolchain != "") {
          toolchain = Controllers::ToolchainManager::by_name(target.toolchain);
        }

        auto [build_result, build_commands] = toolchain.build(target);

        target.compile_commands = build_commands;

        compile_commands.append_range(target.compile_commands);

        if (build_result != BuildOutputResultStatus::Success) {
          throw Core::Exceptions::RuntimeException(
              "Couldn't build project {} with language {} and toolchain {}",
              target.name, target.language, toolchain.name);
        }

      } catch (std::exception &ex) {
        Modules::Logging::LoggerManager::error("Couldn't build the project: {}",
                                               ex.what());
      }
    }

    auto stream = std::ofstream("compile_commands.json");
    Modules::Serialization::JsonOutputArchiver output(stream);
    output % compile_commands;

    return {BuildProjectOutputResultStatus::Success, project_manifest,
            toolchain};
  }

  enum class BuildTargetOutputResultStatus { Success, Failure };
  using BuildTargetOutputResult =
      std::tuple<BuildTargetOutputResultStatus, TargetDescriptor,
                 ToolchainDescriptor>;

  static inline std::optional<TargetDescriptor>
  target_find(const String &name) {
    auto result =
        std::find_if(project_index.begin(), project_index.end(),
                     [&name](auto &&pair) { return name == pair.second.name; });

    if (result != project_index.end()) {
      return result->second;
    }

    return std::nullopt;
  }

  static inline void target_library_sanitize(TargetDescriptor &input) {

    auto result = input.link_libraries |
                  std::views::filter([&input](auto &&link_library) {
                    auto it =
                        std::find_if(project_index.begin(), project_index.end(),
                                     [&link_library](auto &&library) {
                                       return library.first == link_library;
                                     });
                    auto result = (it != project_index.end());

                    if (result == true) {
                      Modules::Logging::LoggerManager::debug(
                          "Sanitizer: removing {} from {} link libraries",
                          link_library, input.name);
                    }

                    return !result;
                  }) |
                  std::ranges::to<Set<String>>();

    input.link_libraries = result;
  }

  static inline BuildTargetOutputResult
  target_build(const TargetDescriptor &input,
               const String &prefix_override = "") {
    Utils::Unused{prefix_override};

    Target target = input;

    auto CXPM_INTERFACE_PATH =
        Utils::Unix::EnvironmentManager::get("CXPM_INTERFACE_PATH");

    target.include_directories_append(
        Set<String>(CXPM_INTERFACE_PATH.begin(), CXPM_INTERFACE_PATH.end()));

    if (!target.link_libraries.empty()) {
      for (const auto &library : target.link_libraries) {
        auto result = target_find(library.c_str());

        if (result.has_value()) {

          auto dependency = result.value();

          target_build(dependency, prefix_override);

          target.include_directories_append(dependency.include_directories);

          target.link_libraries_append(dependency.link_libraries);
          target.link_directories_append(dependency.link_directories);

          target.options_append(dependency.options);
        }
      }
    }

    auto result = ToolchainManager::autoselect_by_language(target.language);

    if (!result.has_value()) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't find toolchain for {} language", target.language);
    }

    Toolchain toolchain = result.value();

    ProjectManager::solve_dependencies(target);
    PackageConfigManager::solve_dependencies(target);

    auto [status, compiler_commands] = toolchain.build(target);

    return {BuildTargetOutputResultStatus::Success, target, toolchain};
  }

  enum class InstallPRojectOutputResultStatus { Success, Failure };

  using InstallProjectOutputResult =
      std::tuple<InstallPRojectOutputResultStatus>;

  static inline InstallProjectOutputResult
  project_install(const Project &target, const String prefix) {
    Utils::Unused{target, prefix};
    return {InstallPRojectOutputResultStatus::Success};
  }
  enum class InstallTargetOutputResultStatus { Success, Failure };

  using InstallTargetOutputResult =
      std::tuple<InstallTargetOutputResultStatus, TargetDescriptor,
                 ToolchainDescriptor>;

  static inline InstallTargetOutputResult
  target_install(TargetDescriptor &target, const ToolchainDescriptor &toolchain,
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
        using namespace CXPM::Models;

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
  target_uninstall(const TargetDescriptor &target, const String &prefix) {
    Utils::Unused{prefix};
    return {UninstallTargetOutputResultStatus::Success, target};
  }

  enum class UninstallProjectOutputResultStatus { Success, Failure };

  using UninstallProjectOutputResult =
      std::tuple<UninstallProjectOutputResultStatus, ProjectDescriptor>;

  static inline UninstallProjectOutputResult
  project_uninstall(const ProjectDescriptor &project, const String &) {

    return {UninstallProjectOutputResultStatus::Success, project};
  }

  enum BuildManifestResultStatus { Success, Failure };

  using BuildManifestResult = std::tuple<BuildManifestResultStatus,
                                         Collection<CompileCommandDescriptor>>;

  static inline BuildManifestResult
  manifest_generate(const Core::Containers::String &project_path) {

    auto manifest = manifest_create();
    manifest.build_path = project_path;

    auto [generate_loader_result, loader_path] = loader_generate(project_path);

    if (generate_loader_result == GenerateLoaderResultStatus::Failure) {
      return BuildManifestResult(BuildManifestResultStatus::Failure,
                                 Collection<CompileCommandDescriptor>());
    }

    auto CXPM_INTERFACE_PATH =
        Utils::Unix::EnvironmentManager::get("CXPM_INTERFACE_PATH");

    manifest.include_directories_append(
        Set<String>(CXPM_INTERFACE_PATH.begin(), CXPM_INTERFACE_PATH.end()));

    auto result = ToolchainManager::autoselect_by_language(manifest.language);

    if (!result.has_value()) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't find toolchain for {} language", manifest.language);
    }

    Toolchain toolchain = result.value();

    auto [status, compiler_commands] = toolchain.build(manifest);

    if (status != BuildOutputResultStatus::Success) {
      return {BuildManifestResultStatus::Failure, compiler_commands};
    }

    return {BuildManifestResultStatus::Success, compiler_commands};
  }

  static inline Models::ProjectDescriptor
  manifest_load_from_path(const Core::Containers::String &manifest_path) {

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

  static inline int project_clean(const String &project_path,
                                  const Collection<String> &) {

    // objects
    for (auto source : DefaultManifestPackage.sources) {
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

  Collection<String> &
  modules_search_paths_append(const Collection<String> &paths) {
    modules_search_paths.append_range(paths);
    return modules_search_paths;
  }

  static inline const Collection<String> &modules_search_paths_get() {
    return modules_search_paths;
  }

  enum class GenerateLoaderResultStatus { Success, Failure };

  using GenerateLoaderResult = std::tuple<GenerateLoaderResultStatus, String>;

  static GenerateLoaderResult loader_generate(std::string base_path) {
    std::ofstream loader_source(base_path + "/package.loader.cpp",
                                std::ios_base::trunc | std::ios_base::out);
    loader_source << BasicProjectLoaderSource;
    loader_source.flush();
    return {
        GenerateLoaderResultStatus::Success,
        std::filesystem::path(base_path).append("package.loader.cpp").string()};
  }

  static inline const Map<String, TargetDescriptor> &targets_get() {
    return project_index;
  }

  static inline void targets_append(const TargetDescriptor &range) {
    // project_index.push_back({range});
    project_index[range.name] = range;
  }

  static inline void targets_append(const Collection<TargetDescriptor> &range) {
    // project_index.append_range(range);

    for (auto target : range) {
      project_index[target.name] = target;
    }
  }

  static inline BuildProjectOutputResult build_all() {

    auto targets = targets_get();

    for (auto [name, target] : targets) {
      target_build(target);
    }

    return {BuildProjectOutputResultStatus::Failure, {}, {}};
  }

  static inline void solve_dependencies(TargetDescriptor &target) {
    auto pkg_config_dependencies =
        target.link_libraries |
        std::views::transform(
            [](auto &&library) -> std::optional<TargetDescriptor> {
              return Controllers::ProjectManager::target_find(library);
            }) |
        std::views::filter([](auto &&result) { return result.has_value(); }) |
        std::views::transform([](auto &&result) { return result.value(); });

    for (auto descriptor : pkg_config_dependencies) {
      target.include_directories.insert_range(descriptor.include_directories);
      target.link_directories.insert_range(descriptor.link_directories);
      target.link_directories
          .insert_range(Set<String>{descriptor.build_path});
              target.link_libraries.insert_range(descriptor.link_libraries);
      target.options.insert_range(descriptor.options);
    }
  }

private:
  static inline Models::Target manifest_create() {
    return DefaultManifestPackage;
  };

  Core::Containers::Collection<Models::ProjectDescriptor> projects;

  static inline Collection<String> modules_search_paths;

  static inline Models::Target DefaultManifestPackage =
      Models::Target()
          .name_set("project-manifest")
          .type_set("shared-library")
          .include_directories_append({
              String(cxpm_BASE_INSTALL_PREFIX) + "/lib/cxpm-base/headers",
              String(cxpm_BASE_INSTALL_PREFIX) + "/share/cxpm-base/headers",
              String(cxpm_BASE_INSTALL_PREFIX) + "/include/cxpm-base",
          })
          .options_append({"-std=c++23", "-Wall", "-Werror", "-pedantic"})
          .sources_append({"package.cpp", "package.loader.cpp"})
          .create();

  static inline const std::string BasicProjectLoaderSource = R"(
    #include <CXPM/Models/Project.hpp>
    using namespace CXPM::Models;
    extern Project project;
    ExportProject(project);
  )";

private:
  static inline Map<String, TargetDescriptor> project_index;
};
} // namespace CXPM::Controllers