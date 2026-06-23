#pragma once

#include "CXPM/TargetInterface.hpp"
#include <CXPM/TargetDescriptor.hpp>
#include <initializer_list>

namespace CXPM::Models {
struct Target : public TargetDescriptor, public TargetInterface<Target> {

  virtual Target &name_set(const String &value) override {
    name = value;
    return *this;
  }
  virtual const String &name_get() const override { return name; }
  virtual Target &version_set(const String &value) override {
    version = value;
    return *this;
  }
  virtual const String &version_get() const override { return version; }
  virtual Target &install_prefix_set(const String &value) override {
    install_prefix = value;
    return *this;
  }
  virtual const String &install_prefix_get() const override {
    return install_prefix;
  }
  virtual Target &
  include_directories_set(const BasicCollection<String> &value) override {
    include_directories = value;
    return *this;
  }
  virtual const BasicCollection<String> &include_directories_get() const override {
    return include_directories;
  }

  virtual Target &include_directories_append(const String &value) {
    include_directories.push_back(value);
    return *this;
  }

  virtual Target &
  include_directories_append(const std::initializer_list<String> &value) {
    include_directories.append_range(value);
    return *this;
  }

  virtual Target &
  link_directories_set(const BasicCollection<String> &value) override {
    link_directories = value;
    return *this;
  }
  virtual const BasicCollection<String> &link_directories_get() const override {
    return link_directories;
  }

  virtual Target &link_directories_append(const String &value) {
    link_directories.push_back(value);
    return *this;
  }

  virtual Target &link_libraries_set(const BasicCollection<String> &value) override {
    link_libraries = value;
    return *this;
  }

  virtual const BasicCollection<String> &link_libraries_get() const override {
    return link_libraries;
  }

  virtual Target &link_libraries_append(const String &value) {
    link_libraries.push_back(value);
    return *this;
  }

  virtual Target &options_set(const BasicCollection<String> &value) override {
    options = value;
    return *this;
  }
  virtual const BasicCollection<String> &options_get() const override {
    return options;
  }

  virtual Target &options_append(const String &value) {
    options.push_back(value);
    return *this;
  }

  virtual Target &options_append(const std::initializer_list<String> &value) {
    options.append_range(value);
    return *this;
  }

  virtual const String &type_get() const override { return type; }

  virtual Target &type_set(const String &value) override {
    type = value;
    return *this;
  }
  virtual const String &url_get() const override { return url; }
  virtual Target &url_set(const String &value) override {
    url = value;
    return *this;
  }
  virtual const String &language_get() const override { return language; }
  virtual Target &language_set(const String &value) override {
    language = value;
    return *this;
  }
  virtual const String &description_get() const override { return description; }
  virtual Target &description_set(const String &value) override {
    description = value;
    return *this;
  }
  virtual const String &toolchain_get() const override { return toolchain; }
  virtual Target &toolchain_set(const String &value) override {
    toolchain = value;
    return *this;
  }
  virtual const BasicCollection<String> &sources_get() const override {
    return sources;
  }
  virtual Target &sources_set(const BasicCollection<String> &value) override {
    sources = value;
    return *this;
  }

  virtual Target &sources_append(const std::initializer_list<String>& value) {
    sources.append_range(value);
    return *this;
  }
  virtual const BasicCollection<String> &dependencies_get() const override {
    return dependencies;
  }
  virtual Target &dependencies_set(const BasicCollection<String> &value) override {
    dependencies = value;
    return *this;
  }
  virtual const BasicCollection<CompileCommandDescriptor> &
  compile_commands_get() const override {
    return compile_commands;
  }
  virtual Target &compile_commands_set(
      const BasicCollection<CompileCommandDescriptor> &value) override {
    compile_commands = value;
    return *this;
  }

  virtual const String& project_path_get() override {
    return project_path;
  }

  virtual Target& project_path_set(const String& value) override {
    project_path = value;
    return *this;
  }

  virtual Target& create() override {
    return *this;
  }
};
} // namespace CXPM::Models