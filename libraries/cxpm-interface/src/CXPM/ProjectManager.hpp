#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/Core/Containers/Collection.hpp"
#include "CXPM/Core/Containers/String.hpp"
#include "CXPM/Core/Exceptions/RuntimeException.hpp"
#include "CXPM/DescriptorSandbox.hpp"
#include "CXPM/Modules/Serialization/JsonManifest.hpp"
#include "CXPM/Modules/Serialization/JsonOutputArchiver.hpp"
#include "CXPM/Project.hpp"
#include "CXPM/ProjectDescriptor.hpp"
#include "CXPM/Target.hpp"
#include "CXPM/TargetDescriptor.hpp"
#include "CXPM/Toolchain.hpp"
#include "CXPM/ToolchainDescriptor.hpp"
#include <CXPM/ToolchainManager.hpp>
#include <CXPM/Utils/Unused.hpp>
#include <dlfcn.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <tuple>

#ifndef cxpm_BASE_INSTALL_PREFIX
#define cxpm_BASE_INSTALL_PREFIX "/usr/local"
#endif

#ifndef cxpm_BASE_SOURCE_PREFIX
#define cxpm_BASE_SOURCE_PREFIX "/usr/src/cxpm/applications/cxpm"
#endif

#ifndef cxpm_BUILD_INSTALL_PREFIX
#define cxpm_BUILD_INSTALL_PREFIX "/usr"
#endif

#ifndef cxpm_DEBUG_BUILD_PREFIX
#define cxpm_DEBUG_BUILD_PREFIX ""
#endif

#ifndef cxpm_DEBUG_PROJECT_PREFIX
#define cxpm_DEBUG_PROJECT_PREFIX ""
#endif

namespace CXPM::Controllers {

class ProjectManager final {

StaticClass(ProjectManager)

    public :

    static inline void initialize() {
    modules_search_paths =
        CXPM::Utils::Unix::EnvironmentManager::get("cxpm_BUILD_EXTRA_MODULES_PATH");

    extra_module_path_add("/usr/share/cxpm/toolchains");
    extra_module_path_add("/usr/local/share/cxpm/toolchains");

    extra_module_path_add("/usr/lib/cxpm/toolchains");
    extra_module_path_add("/usr/local/lib/cxpm/toolchains");

    extra_module_path_add(cxpm_BASE_INSTALL_PREFIX "/lib/cxpm/toolchains");
    extra_module_path_add(cxpm_BASE_INSTALL_PREFIX "/share/cxpm/toolchains");
  }

  using BuildProjectOutputResult =
      std::tuple<Status, ProjectDescriptor, ToolchainDescriptor>;

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

  static inline BasicCollection<String> modules_search_paths;

  static inline BuildProjectOutputResult build_project(const String directory) {

    Toolchain toolchain;
    BasicCollection<CompileCommandDescriptor> commands;

    auto build_path = std::filesystem::path(directory.c_str());

    if (!directory.empty()) {
      build_path = directory.c_str();
    }

    // Toolchain discovery (ToolchainManager::autoscan) used to happen only as a side effect of
    // ToolchainManager::current() being called from inside build_manifest() to pick a toolchain
    // for compiling package.cpp itself. That made toolchain discovery silently dependent on the
    // manifest being a package.cpp; a package.json manifest never triggered it at all, so
    // autoselect() below would see an empty toolchain registry. Call current() exactly once,
    // up front, so both manifest formats populate the toolchain registry identically, and pass
    // its result into load_project() instead of letting build_manifest() call current() again
    // (which would otherwise re-run autoscan's toolchain JIT-compilation a second time).
    auto manifest_toolchain = CXPM::Controllers::ToolchainManager::current(
        build_path.string(), modules_search_paths);

    auto project_manifest = Controllers::ProjectManager::load_project(
        build_path.string(), manifest_toolchain, commands);

    for (auto toolchain : project_manifest.toolchains) {
      try {

        if (CXPM::Controllers::ToolchainManager::valid(toolchain)) {
          CXPM::Controllers::ToolchainManager::add(toolchain);
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error(
            "Couldn't append toolchain specified in project {}", ex.what());
      }
    }

    for (auto target : project_manifest.targets) {
      try {

        toolchain = CXPM::Controllers::ToolchainManager::autoselect(target);

        if (target.toolchain != "") {
          toolchain = CXPM::Controllers::ToolchainManager::by_name(target.toolchain);
        }

        // fix project path
        target.project_path = directory;

        auto [build_result, build_commands] = toolchain.build(target);

        target.compile_commands = build_commands;

        commands.append_range(target.compile_commands);

        if (build_result != Status::Success) {
          throw Core::Exceptions::RuntimeException(
              "Couldn't build project {} with language {} and toolchain {}",
              target.name, target.language, toolchain.name);
        }

      } catch (std::exception &ex) {
        Core::Logging::LoggerManager::error("Couldn't build the project: {}",
                                            ex.what());
      }
    }

    auto compile_commands_path = std::filesystem::path()
                                     .append(directory.c_str())
                                     .append("compile_commands.json");

    auto stream =
        std::ofstream(std::filesystem::absolute(compile_commands_path));

    {
      using namespace CXPM::Modules::Serialization;
      auto joa = JsonOutputArchiver(stream);
      joa % ValueToken{commands};
    }

    return {Status::Success, project_manifest, toolchain};
  }

  // Loads a project's manifest, dispatching on which serialization alternative is present in
  // `project_path` (see docs/SRS-json-manifests.md): a package.cpp is compiled into a shared
  // object and dlopen'd back exactly as before; a package.json is parsed directly, with no
  // compilation or dlopen involved at all. package.cpp takes precedence when both exist, so
  // adding a package.json alongside an existing package.cpp is never a silent behavior change.
  static inline ProjectDescriptor
  load_project(const String &project_path,
              const ToolchainDescriptor &manifest_toolchain,
              BasicCollection<CompileCommandDescriptor> &manifest_compile_commands) {

    auto cpp_manifest_path =
        std::filesystem::path(project_path.c_str()) / "package.cpp";
    auto json_manifest_path =
        std::filesystem::path(project_path.c_str()) / "package.json";

    if (std::filesystem::exists(cpp_manifest_path)) {
      auto [build_manifest_result, build_manifest_commands] =
          build_manifest(project_path, manifest_toolchain);

      manifest_compile_commands.append_range(build_manifest_commands);

      if (build_manifest_result != BuildManifestResultStatus::Success) {
        throw Core::Exceptions::RuntimeException(
            "Failed to build manifest {} using {}",
            (std::filesystem::path(project_path.c_str()) /
             "libproject-manifest.so")
                .string(),
            cpp_manifest_path.string());
      }

      auto project = load_from_manifest(
          (std::filesystem::path(project_path.c_str()) /
           "libproject-manifest.so")
              .string());
      project.compile_comands = build_manifest_commands;
      return project;
    }

    if (std::filesystem::exists(json_manifest_path)) {
      std::ifstream stream(json_manifest_path);
      if (!stream) {
        throw Core::Exceptions::RuntimeException(
            "Couldn't open project manifest {}", json_manifest_path.string());
      }
      std::ostringstream buffer;
      buffer << stream.rdbuf();

      using namespace CXPM::Modules::Serialization;
      return project_descriptor_from_json(
          parse_json(String(buffer.str())));
    }

    throw Core::Exceptions::RuntimeException(
        "Couldn't find package.cpp or package.json in {}", project_path);
  }

  using BuildTargetOutputREsult =
      std::tuple<Status, TargetDescriptor, ToolchainDescriptor>;

  static inline BuildTargetOutputREsult
  build_target(const TargetDescriptor &target, const String &prefix_override) {
    Utils::Unused{prefix_override};
    Toolchain toolchain;
    return {Status::Success, target, toolchain};
  }

  using InstallProjectOutputResult = std::tuple<Status>;

  static inline InstallProjectOutputResult
  install_project(const Project &target, const String prefix) {
    Utils::Unused{target, prefix};
    return {Status::Failure};
  }

  using InstallTargetOutputResult =
      std::tuple<Status, TargetDescriptor, ToolchainDescriptor>;

  static inline InstallTargetOutputResult
  install_target(TargetDescriptor &target, const ToolchainDescriptor &toolchain,
                 String prefix_override = "") {
    // include directory should be in project directory
    using directory_iterator = std::filesystem::directory_iterator;

    String prefix = toolchain.install_prefix;

    if (!prefix_override.empty()) {
      prefix = prefix_override;
    }

    // Resolves a path that may be relative to target.project_path -- exactly the convention
    // Toolchain::object_build() already uses for include directories (Toolchain.hpp). Every
    // artifact path below (the built binary/library itself, and each include directory) is
    // ordinarily relative (e.g. "src", the target's bare name) once a real `cxpm --build` has
    // already run in a different working directory than install_target() runs in; resolving
    // against project_path instead of relying on the process's current directory is a fix for a
    // real bug: install_target() previously used every such path completely unresolved, working
    // only by accident when the caller's current working directory happened to already be the
    // project directory (which ApplicationView::install_project never arranges).
    auto resolve_project_path = [&target](const String &relative) {
      auto path = std::filesystem::path(relative.c_str());
      if (path.is_absolute()) {
        return path;
      }
      return std::filesystem::path(target.project_path.c_str()) / relative.c_str();
    };

    // Directory setup, artifact install (executable/shared-library/static-library/
    // object-library) and .pc generation used to be nested inside the "for each include
    // directory" loop below, which meant a target with an *empty* include_directories list
    // (a legal, otherwise-unremarkable configuration) silently installed nothing at all --
    // not even its own binary. They now run exactly once regardless of how many include
    // directories (zero, one, or many) the target has; only header copying is inherently
    // per-directory.
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

    // install headers from every include directory
    for (auto directory : target.include_directories) {

      for (auto file : directory_iterator(resolve_project_path(directory))) {

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
    }

    if (target.type == "executable") {
      std::filesystem::copy(
          resolve_project_path(target.name),
          std::filesystem::path(binnaries_install_path),
          std::filesystem::copy_options::overwrite_existing);
    }

    if (target.type == "shared-library") {
      std::filesystem::copy(
          resolve_project_path("lib" + target.name + ".so"),
          std::filesystem::path(library_install_path),
          std::filesystem::copy_options::overwrite_existing);
    }

    if (target.type == "static-library") {
      std::filesystem::copy(
          resolve_project_path("lib" + target.name + ".a"),
          std::filesystem::path(archive_install_path),
          std::filesystem::copy_options::overwrite_existing);
    }

    if (target.type == "object-library") {
      std::filesystem::copy(
          resolve_project_path(target.name + ".o"),
          std::filesystem::path(library_install_path),
          std::filesystem::copy_options::overwrite_existing);
    }

    // install pc file
    {
      using namespace CXPM;

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

    return InstallTargetOutputResult{Status::Success, target, toolchain};
  }

  using UninstallTargetOutputResult = std::tuple<Status, TargetDescriptor>;

  static inline UninstallTargetOutputResult
  uninstall_target(const TargetDescriptor &target, const String &prefix) {
    Utils::Unused{prefix};
    return {Status::Success, target};
  }

  using UninstallProjectOutputResult = std::tuple<Status, ProjectDescriptor>;

  static inline UninstallProjectOutputResult
  uninstall_project(const ProjectDescriptor &project, const String &) {

    return {Status::Success, project};
  }

  enum BuildManifestResultStatus { Success, Failure };

  using BuildManifestResult =
      std::tuple<BuildManifestResultStatus,
                 BasicCollection<CompileCommandDescriptor>>;

  static inline BuildManifestResult
  build_manifest(const String &project_path,
                 const ToolchainDescriptor &manifest_toolchain) {

    auto [generate_loader_result, loader_path] = generate_loader(project_path);

    if (generate_loader_result == Status::Failure) {
      return BuildManifestResult(BuildManifestResultStatus::Failure,
                                 BasicCollection<CompileCommandDescriptor>());
    }

    auto manifest_project = ManifestPackage;
    manifest_project.project_path_set(project_path);

    auto [loader_build_result, loader_compile_commands] =
        Toolchain(manifest_toolchain).build(manifest_project);

    auto manifest_status = loader_build_result == Status::Success
                               ? BuildManifestResultStatus::Success
                               : BuildManifestResultStatus::Failure;

    return {manifest_status, loader_compile_commands};
  }

  static inline ProjectDescriptor
  load_from_manifest(const String &manifest_path) {

    // manifest_path was just compiled from this project's own package.cpp; sandbox loading it
    // by default rather than dlopen()-ing it directly into this process (see
    // docs/SRS-sandbox.md). CXPM_SANDBOX_DISABLE=1 restores the pre-sandbox direct-dlopen
    // behavior below.
    if (CXPM::Controllers::DescriptorSandbox::enabled()) {
      return CXPM::Controllers::DescriptorSandbox::load_project(manifest_path);
    }

    typedef Project *(*getter_type)();

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
                          const BasicCollection<String> &) {

    // objects. source + ".o" is concatenated into one path component *before* being appended,
    // since std::filesystem::path::append() inserts a directory separator between components --
    // appending source.c_str() and ".o" as two separate append() calls (as this used to do)
    // built "<project_path>/<source>/.o" instead of the intended "<project_path>/<source>.o".
    for (auto source : ManifestPackage.sources) {
      std::filesystem::remove_all(
          std::filesystem::path()
              .append(project_path.c_str())
              .append(String(source + ".o").c_str()));
    }

    // generate packages
    std::filesystem::remove_all(std::filesystem::path()
                                    .append(project_path.c_str())
                                    .append("package.cpp.o"));

    std::filesystem::remove_all(std::filesystem::path()
                                    .append(project_path.c_str())
                                    .append("package.loader.cpp.o"));

    std::filesystem::remove_all(std::filesystem::path()
                                    .append(project_path.c_str())
                                    .append("package.loader.cpp"));

    // generated toolchain plugins
    if (std::filesystem::is_directory(project_path.c_str())) {
      using directory_iterator = std::filesystem::directory_iterator;
      for (auto entry : directory_iterator(project_path.c_str())) {
        auto filename = entry.path().filename().string();

        if ((filename.starts_with("toolchain-") &&
             filename.ends_with(".loader.cpp")) ||
            (filename.starts_with("libtoolchain-") &&
             filename.ends_with(".so"))) {
          std::filesystem::remove_all(entry.path());
        }
      }
    }

    return 0;
  }

  BasicCollection<ProjectDescriptor> projects;

private:
  static inline const Target ManifestPackage =
      Target()
          .name_set("project-manifest")
          .type_set("shared-library")
          .include_directories_append({
              (std::filesystem::path(cxpm_BASE_INSTALL_PREFIX) /
               "lib/cxpm/headers")
                  .string(),
              (std::filesystem::path(cxpm_BASE_INSTALL_PREFIX) /
               "share/cxpm/headers")
                  .string(),
              (std::filesystem::path(cxpm_BASE_INSTALL_PREFIX) / "include")
                  .string(),
              (std::filesystem::path(cxpm_BASE_SOURCE_PREFIX) / "src")
                  .string(),
          })
          .options_append({"-std=c++23", "-Wall", "-Werror", "-pedantic"})
          .sources_append({"package.cpp", "package.loader.cpp"});

  static inline const std::string BasicProjectLoaderSource = R"(
    #include <CXPM/ProjectDescriptor.hpp>
    using namespace CXPM;
    extern ProjectDescriptor project;
    // should be a weak reference that can be overriten by a custom get_project // more versatile but unsafe.
    extern "C" const ProjectDescriptor* get_project()  { return &project; }
  )";

  using GenerateLoaderResult = std::tuple<Status, String>;

  static GenerateLoaderResult generate_loader(std::string base_path) {
    std::ofstream loader_source(base_path + "/package.loader.cpp",
                                std::ios_base::trunc | std::ios_base::out);
    loader_source << BasicProjectLoaderSource;
    loader_source.flush();
    return {
        Status::Success,
        std::filesystem::path(base_path).append("package.loader.cpp").string()};
  }
};
} // namespace Controllers