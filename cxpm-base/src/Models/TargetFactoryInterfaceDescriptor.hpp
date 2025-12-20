#pragma once

#include "Models/TargetDescriptor.hpp"
#include <Core/Containers/String.hpp>
#include <iterator>
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
    std::copy(options.begin(), options.end(),
              std::back_inserter(this->options));
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  link_directories_append(const Collection<String> &paths) {
    this->link_directories.push_back(paths.begin(), paths.end());
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  link_libraries_append(const Collection<String> &link_libraries) {
    this->link_libraries.push_back(link_libraries.begin(),
                                   link_libraries.end());
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  include_directories_append(const Collection<String> &paths) {
    std::copy(paths.begin(), paths.end(),
              std::back_inserter(this->include_directories));
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  dependencies_append(const String &dependency) {
    this->dependencies.push_back(dependency);
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  dependencies_append(const Collection<String> &paths) {
    this->dependencies.push_back(paths.begin(), paths.end());
    return *this;
  }

  constexpr TargetDescriptorInterface<DerivedType> &
  sources_append(const Collection<String> &sources) {
    std::copy(sources.begin(), sources.end(),
              std::back_inserter(this->sources));
    return *this;
  }

  constexpr const DerivedType &create() {
    return static_cast<DerivedType &>(*this);
  }
};
} // namespace Models