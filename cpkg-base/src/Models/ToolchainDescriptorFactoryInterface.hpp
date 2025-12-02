#pragma once

#include <Models/ProjectDescriptor.hpp>
#include <Models/ToolchainDescriptor.hpp>

#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Containers/Variant.hpp>

using namespace Core::Containers;

namespace Models {
template <typename DerivedType>
class ToolchainDescriptorFactoryInterface : public ToolchainDescriptor {
public:
  using ToolchainDescriptor::ToolchainDescriptor;

  template <typename... ArgsTypes>
  ToolchainDescriptorFactoryInterface(ArgsTypes &&...args)
      : ToolchainDescriptor(std::forward<ArgsTypes>(args)...) {}

  ToolchainDescriptorFactoryInterface<DerivedType> &
  name_set(const Core::Containers::String &value) {
    name = value;
    return *this;
  }
  Core::Containers::String name_get() { return name; };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  version_set(const Core::Containers::String &value) {
    version = value;
    return *this;
  }
  Core::Containers::String version_get() { return version; };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  language_set(const Core::Containers::String &value) {
    language = value;
    return *this;
  }
  Core::Containers::String language_get() { return language; };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  install_prefix_set(const Core::Containers::String &value) {
    install_prefix = value;
    return *this;
  }
  Core::Containers::String install_prefix_get() { return install_prefix; };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  include_directory_prefix_set(const Core::Containers::String &value) {
    include_directory_prefix = value;
    return *this;
  }
  Core::Containers::String include_directory_prefix_get() {
    return include_directory_prefix;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  include_directories_set(const Collection<Core::Containers::String> &value) {
    include_directories = value;
    return *this;
  }
  Collection<Core::Containers::String> include_directories_get() {
    return include_directories;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  compiler_executable_set(const Core::Containers::String &value) {
    compiler_executable = value;
    return *this;
  }
  Core::Containers::String compiler_executable_get() {
    return compiler_executable;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  compiler_options_set(const Collection<Core::Containers::String> &value) {
    compiler_options = value;
    return *this;
  }
  Collection<Core::Containers::String> compiler_options_get() {
    return compiler_options;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  linker_executable_set(const Core::Containers::String &value) {
    linker_executable = value;
    return *this;
  }
  Core::Containers::String linker_executable_get() {
    return linker_executable;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  archiver_executable_set(const Core::Containers::String &value) {
    archiver_executable = value;
    return *this;
  }
  Core::Containers::String archiver_executable_get() {
    return archiver_executable;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  archiver_options_set(const Collection<Core::Containers::String> &value) {
    archiver_options = value;
    return *this;
  }
  Collection<Core::Containers::String> archiver_options_get() {
    return archiver_options;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  linker_options_set(const Collection<Core::Containers::String> &value) {
    linker_options = value;
    return *this;
  }
  Collection<Core::Containers::String> linker_options_get() {
    return linker_options;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  link_directory_prefix_set(const Core::Containers::String &value) {
    link_directory_prefix = value;
    return *this;
  }
  Core::Containers::String link_directory_prefix_get() {
    return link_directory_prefix;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  link_library_prefix_set(const Core::Containers::String &value) {
    link_library_prefix = value;
    return *this;
  }
  Core::Containers::String link_library_prefix_get() {
    return link_library_prefix;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  source_specifier_prefix_set(const Core::Containers::String &value) {
    source_specifier_prefix = value;
    return *this;
  }
  Core::Containers::String source_specifier_prefix_get() {
    return source_specifier_prefix;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  object_specifier_prefix_set(const Core::Containers::String &value) {
    object_specifier_prefix = value;
    return *this;
  }
  Core::Containers::String object_specifier_prefix_get() {
    return object_specifier_prefix;
  };

  ToolchainDescriptorFactoryInterface<DerivedType> &
  link_directories_set(const Collection<Core::Containers::String> &value) {
    link_directories = value;
    return *this;
  }
  Collection<Core::Containers::String> link_directories_get() {
    return link_directories;
  };

  DerivedType &create() { return static_cast<DerivedType &>(*this); }
};
} // namespace Models