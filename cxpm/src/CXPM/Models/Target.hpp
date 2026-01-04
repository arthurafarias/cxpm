#pragma once

#include <CXPM/Models/TargetDescriptor.hpp>

namespace CXPM::Models {
struct Target : public TargetDescriptor {
  using TargetDescriptor::TargetDescriptor;
  constexpr Target &name_set(const Core::Containers::String &name) {
    this->name = name;
    return *this;
  }
  constexpr Target &version_set(const Core::Containers::String &version) {
    this->version = version;
    return *this;
  }

  constexpr Target &language_set(const Core::Containers::String &language) {
    this->language = language;
    return *this;
  }
  constexpr Target &type_set(const Core::Containers::String &type) {
    this->type = type;
    return *this;
  }

  constexpr Target &options_append(const Collection<String> &options) {
    this->options.append_range(options);
    return *this;
  }

  constexpr Target &link_directories_append(const Collection<String> &paths) {
    this->link_directories.append_range(paths);
    return *this;
  }

  constexpr Target &
  link_libraries_append(const Collection<String> &link_libraries) {
    this->link_libraries.append_range(link_libraries);
    return *this;
  }

  constexpr Target &
  include_directories_append(const Collection<String> &paths) {
    this->include_directories.append_range(paths);
    return *this;
  }

  constexpr Target &dependencies_append(const TargetDescriptor &dependency) {
    this->dependencies.push_back(dependency);
    return *this;
  }
  
  constexpr Target &sources_append(const Collection<String> &sources) {
    this->sources.append_range(sources);
    return *this;
  }

  constexpr const Target &create() { return *this; }
};
} // namespace CXPM::Models