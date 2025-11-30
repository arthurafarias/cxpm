#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Models/BasicProjectDescriptor.hpp"
#include "Models/ProjectDescriptor.hpp"
#include "Models/TargetDescriptor.hpp"
#include <Controllers/ToolchainManager.hpp>
#include <Core/Containers/String.hpp>
#include <dlfcn.h>

#include <fstream>

// configs
#ifndef CPKG_BUILD_HEADERS_PATH
#define CPKG_BUILD_HEADERS_PATH "./"
#endif

#ifndef CPKG_BUILD_INSTALL_HEADERS_PATH
#define CPKG_BUILD_INSTALL_HEADERS_PATH "/usr/share/cpkg-build/private-headers/"
#endif

#ifndef CPKG_BASE_HEADERS_PATH
#define CPKG_BASE_HEADERS_PATH "./"
#endif

#ifndef CPKG_BASE_INSTALL_HEADERS_PATH
#define CPKG_BASE_INSTALL_HEADERS_PATH "/usr/share/cpkg-base/private-headers/"
#endif

namespace Controllers {

struct ProjectManager {

  static inline int
  build_manifest(const Core::Containers::String &project_path,
                 Core::Containers::Collection<Core::Containers::String>
                     extra_toolchain_search_paths) {

    if (__generate_loader(project_path) != 0) {
      return -1;
    }

    if (Controllers::ToolchainManager::current().build(ManifestPackage) != 0) {
      return -1;
    }

    return 0;
  }

  static inline Models::BasicProjectDescriptor
  load_from_manifest(const Core::Containers::String &manifest_path) {

    typedef Models::ProjectDescriptor *(*getter_type)();

    void *handle = dlopen("./project-manifest.so", RTLD_NOW | RTLD_DEEPBIND);

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

  Core::Containers::Collection<Models::BasicProjectDescriptor> projects;

private:

  static inline Models::TargetDescriptor ManifestPackage =
      Models::TargetDescriptor()
          .name_set("project-manifest")
          .type_set("shared-library")
          .include_directories_append(
              {CPKG_BUILD_INSTALL_HEADERS_PATH, CPKG_BUILD_HEADERS_PATH,
               CPKG_BASE_HEADERS_PATH, CPKG_BASE_INSTALL_HEADERS_PATH})
          .options_append({"-std=c++23", "-Wall", "-Werror", "-pedantic"})
          .sources_append({"package.cpp", "package.loader.cpp"})
          .create();

  static inline const std::string BasicProjectLoaderSource = R"(
    #include <Models/BasicProjectDescriptor.hpp>
    using namespace Models;
    extern BasicProjectDescriptor project;
    // should be a weak reference that can be overriten by a custom get_project // more versatile but unsafe.
    extern "C" const BasicProjectDescriptor* get_project()  { return &project; }
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