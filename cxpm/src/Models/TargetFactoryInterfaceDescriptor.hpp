#pragma once

#include "Models/TargetDescriptor.hpp"
#include <Core/Containers/String.hpp>
using namespace Core::Containers;
namespace Models {

template <typename DerivedType>
class TargetDescriptorInterface : public TargetDescriptor {
public:
  using TargetDescriptor::TargetDescriptor;
  constexpr TargetDescriptorInterface<DerivedType> &
  name_set(const Core::Containers::String &name) {
    this->name = name;
    return *this;
  }
  constexpr TargetDescriptorInterface<DerivedType> &
  version_set(const Core::Containers::String &version) {
    this->version = version;
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  language_set(const Core::Containers::String &language) {
    this->language = language;
    return *this;
  }
  constexpr TargetDescriptorInterface<DerivedType> &
  type_set(const Core::Containers::String &type) {
    this->type = type;
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  options_append(const Collection<String> &options) {
    this->options.append_range(options);
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  link_directories_append(const Collection<String> &paths) {
    this->link_directories.append_range(paths);
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  link_libraries_append(const Collection<String> &link_libraries) {
    this->link_libraries.append_range(link_libraries);
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  include_directories_append(const Collection<String> &paths) {
    this->include_directories.append_range(paths);
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  dependencies_append(const TargetDescriptor &dependency) {
    this->dependencies.push_back(dependency.name);
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  dependencies_append(const String &dependency) {
    this->dependencies.push_back(dependency);
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  dependencies_append(const Collection<String> &paths) {
    this->dependencies.append_range(paths);
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  sources_append(const Collection<String> &sources) {
    this->sources.append_range(sources);
    return *this;
  }

  constexpr const DerivedType &create() {
    return static_cast<DerivedType &>(*this);
  }
};
} // namespace Models