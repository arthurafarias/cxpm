#pragma once

#include <Core/Containers/Variant.hpp>
using namespace Core::Containers;
#include <deque>

namespace Models {
class BasicTarget {
public:
  Variant name = "";
  Variant version = "";
  Variant type = "";
  Variant url = "";
  Variant language = "c++";
  Variant toolchain = "";
  std::deque<Variant> dependencies;
  std::deque<Variant> include_directories;
  std::deque<Variant> sources;
  std::deque<Variant> options;
  std::deque<Variant> link_libraries;
  std::deque<Variant> link_directories;
};

class BasicTargetFactory {
public:
  static inline BasicTargetFactory instance() { return BasicTargetFactory(); }
  constexpr BasicTargetFactory &name(const std::string &name) {
    this->target.name = name;
    return *this;
  }
  constexpr BasicTargetFactory &version(const std::string &version) {
    this->target.version = version;
    return *this;
  }

  constexpr BasicTargetFactory &language(const std::string &language) {
    this->target.language = language;
    return *this;
  }
  constexpr BasicTargetFactory &type(const std::string &type) {
    this->target.type = type;
    return *this;
  }
  constexpr BasicTargetFactory &
  dependencies(const std::deque<std::string> &dependencies) {
    this->target.dependencies.append_range(dependencies);
    return *this;
  }
  constexpr BasicTargetFactory &
  options(const std::deque<std::string> &options) {
    this->target.options.append_range(options);
    return *this;
  }

  constexpr BasicTargetFactory &
  sources(const std::deque<std::string> &sources) {
    this->target.sources.append_range(sources);
    return *this;
  }

  /** this will be removed! not necessary if dependency is correctly configured
   * under the build system */
  constexpr BasicTargetFactory &
  link_libraries(const std::deque<std::string> &link_libraries) {
    this->target.link_libraries.append_range(link_libraries);
    return *this;
  }
  constexpr BasicTargetFactory &
  link_directories(const std::deque<std::string> &link_directories) {
    this->target.link_directories.append_range(link_directories);
    return *this;
  }
  constexpr BasicTarget create() { return target; }

private:
  BasicTarget target;
};

} // namespace Models