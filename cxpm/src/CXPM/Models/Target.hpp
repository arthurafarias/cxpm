#pragma once

#include <ranges>

#include <CXPM/Models/TargetDescriptor.hpp>

namespace CXPM::Models {
struct Target : public TargetDescriptor {
  using TargetDescriptor::TargetDescriptor;

  Target() : TargetDescriptor() {}
  Target(const TargetDescriptor &target) : TargetDescriptor(target) {}

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

  constexpr Target &options_append(const Set<String> &options) {
    auto range =
        options | std::views::filter([](auto &&el) { return !el.empty(); });
    this->options.insert_range(range);
    return *this;
  }

  constexpr Target &link_directories_append(const Set<String> &paths) {
    auto range =
        paths | std::views::filter([](auto &&el) { return !el.empty(); });
    this->link_directories.insert_range(range);
    return *this;
  }

  constexpr Target &
  link_libraries_append(const Set<String> &link_libraries) {
    auto range = link_libraries |
                 std::views::filter([](auto &&el) { return !el.empty(); });
    this->link_libraries.insert_range(range);
    return *this;
  }

  constexpr Target &
  include_directories_append(const Set<String> &paths) {
    auto range =
        paths | std::views::filter([](auto &&el) { return !el.empty(); });
    this->include_directories.insert_range(range);
    return *this;
  }

  constexpr Target &sources_append(const Set<String> &sources) {
    auto range =
        sources | std::views::filter([](auto &&el) { return !el.empty(); });
    this->sources.insert_range(range);
    return *this;
  }

  constexpr const Target &create() { return *this; }

  bool ready = false;
};
} // namespace CXPM::Models