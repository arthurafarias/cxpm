#pragma once

#include <Core/Containers/Variant.hpp>
using namespace Core::Containers;

namespace Models {
struct ToolchainDescriptor {
  Variant name;
  Variant version;
  Variant language;

  Variant install_prefix;

  std::deque<Variant> include_directories;

  Variant compiler_executable;
  std::deque<Variant> compiler_options;

  Variant linker_executable;
  std::deque<Variant> linker_options;

  std::deque<Variant> link_directories;
};

class BasicToolchainFactory {
public:
  static BasicToolchainFactory instance() { return BasicToolchainFactory(); }

  BasicToolchainFactory name(const std::string &name) {
    toolchain.name = name;
    return *this;
  }

  BasicToolchainFactory version(const std::string &version) {
    toolchain.version = version;
    return *this;
  }

  BasicToolchainFactory language(const std::string &language) {
    toolchain.language = language;
    return *this;
  }

  BasicToolchainFactory install_prefix(const std::string &install_prefix) {
    toolchain.install_prefix = install_prefix;
    return *this;
  }

  BasicToolchainFactory
  include_directories(const std::deque<std::string> &include_directories) {
    toolchain.include_directories.append_range(include_directories);
    return *this;
  }

  BasicToolchainFactory
  compiler_executable(const std::string &compiler_executable) {
    toolchain.compiler_executable = compiler_executable;
    return *this;
  }

  BasicToolchainFactory
  compiler_options(const std::deque<std::string> &compiler_options) {
    toolchain.compiler_options.append_range(compiler_options);
    return *this;
  }

  BasicToolchainFactory
  linker_executable(const std::string &linker_executable) {
    toolchain.linker_executable = linker_executable;
    return *this;
  }

  BasicToolchainFactory
  linker_options(const std::deque<std::string> &linker_options) {
    toolchain.linker_options.append_range(linker_options);
    return *this;
  }

  BasicToolchainFactory
  link_directories(const std::deque<std::string> &link_directories) {
    toolchain.link_directories.append_range(link_directories);
    return *this;
  }

  ToolchainDescriptor build() { return toolchain; }

private:
  ToolchainDescriptor toolchain;
};

} // namespace Models