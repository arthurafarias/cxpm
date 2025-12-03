#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Models/BuildOutputResult.hpp"
#include "Models/Project.hpp"
#include "Models/ProjectDescriptor.hpp"
#include "Models/Target.hpp"
#include <Controllers/ToolchainManager.hpp>
#include <Core/Containers/String.hpp>
#include <dlfcn.h>

#include <fstream>

#ifndef CPKG_BASE_INSTALL_PREFIX
#define CPKG_BASE_INSTALL_PREFIX "/usr/local"
#endif

#ifndef CPKG_BASE_SOURCE_PREFIX
#define CPKG_BASE_SOURCE_PREFIX "/usr/src/cpkg/cpkg-base"
#endif

namespace Controllers {

class ProjectManager final {

StaticClass(ProjectManager)

    public :

    static inline BuildOutputResult
    build_manifest(const Core::Containers::String &project_path,
                   Core::Containers::Collection<Core::Containers::String>
                       extra_toolchain_search_paths) {

    if (__generate_loader(project_path) != 0) {
      return BuildOutputResult(-1, Collection<CompileCommandDescriptor>());
    }

    return Controllers::ToolchainManager::current(extra_toolchain_search_paths)
        .build(ManifestPackage);
  }

  static inline Models::ProjectDescriptor
  load_from_manifest(const Core::Containers::String &manifest_path) {

    typedef Models::Project *(*getter_type)();

    void *handle = dlopen(manifest_path.c_str(), RTLD_NOW | RTLD_DEEPBIND);

    if (handle == nullptr) {
      throw Core::Exceptions::RuntimeException(
          "Couldn't load manifest at path {}: not found!",
          manifest_path.c_str());
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

  static inline int clean(const Core::Containers::String &project_path,
                          Core::Containers::Collection<Core::Containers::String>
                              extra_toolchain_search_paths) {

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
              String(CPKG_BASE_INSTALL_PREFIX) + "/lib/cpkg-base/headers",
              String(CPKG_BASE_INSTALL_PREFIX) + "/share/cpkg-base/headers",
              String(CPKG_BASE_INSTALL_PREFIX) + "/include/cpkg-base",
              String(CPKG_BASE_SOURCE_PREFIX) + "/src",
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

  static int __generate_loader(std::string base_path) {
    std::ofstream loader_source(base_path + "/package.loader.cpp",
                                std::ios_base::trunc | std::ios_base::out);
    loader_source << BasicProjectLoaderSource;
    loader_source.flush();
    return 0;
  }
};
} // namespace Controllers