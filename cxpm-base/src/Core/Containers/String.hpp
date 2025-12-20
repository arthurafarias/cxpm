#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Object.hpp"
#include "Utils/StringUtils/FormatString.hpp"
#include "Utils/Unused.hpp"
#include <cctype>
#include <string>
#include <utility>

namespace Core {
namespace Containers {

class String : public std::string, public Object {

public:
  using std::string::string;

  String(const std::string &other) : std::string(other) {}

  template <typename... ArgumentTypes>
  String(const ArgumentTypes &&...args)
      : std::string(std::forward<const ArgumentTypes>(args)...) {}

  inline static String join(const Collection<String> &collection,
                            String delimiter) {

    if (collection.size() == 0) {
      return "";
    }

    String result = collection.front();

    for (auto el = collection.begin() + 1; el < collection.end(); el++) {
      result += delimiter + *el;
    }

    return result;
  }

  static inline String trim(const String &source) {
    auto _left_trimmed = String::trim_left(source);
    auto _right_trimmed = String::trim_left(_left_trimmed);
    return String::trim_right(_right_trimmed);
  }

  static inline String trim_left(const String &source) {
    size_t first = 0;
    for (; first < source.size(); first++) {
      if (std::isprint((source)[first])) {
        break;
      }
    }

    return source.substr(first);
  }

  static inline String trim_right(const String &source) {
    if (source.empty())
      return source; // Handle empty string edge case
    int end = source.size() - 1;
    for (; end >= 0; end--) {
      if (std::isprint((source)[end])) {
        break;
      }
      if (end == 0)
        break; // Prevents underflow on unsigned size_t
    }

    return source.substr(0,
                         end + 1); // end + 1 because substr length is exclusive
  }

  Collection<String> split(String needle) const {
    auto lock = acquire_lock();
    Collection<String> splitted;
    auto &haystack = *this;
    size_t position_last = 0;
    size_t position = 0;

    while ((position = haystack.find(needle, position)) != (size_t)(-1)) {
      auto element = String::trim(
          haystack.substr(position_last, position - position_last));
      splitted.push_back(element);
      position_last = position + needle.length(); // Skip past the needle
      position++; // Move past the needle for the next search
    }

    // Add the remainder of the string after the last split
    if (position_last < haystack.size()) {
      auto element = String::trim(haystack.substr(position_last));
      splitted.push_back(element);
    }

    return splitted;
  }

  bool starts_with(const String &other) { return find(other) == 0; }

  bool ends_with(const String &other) { return rfind(other) == 0; }

  template <typename... ArgsTypes>
  static inline String format(const Utils::StringUtils::FormatString &,
                              ArgsTypes &&...args) {
    Utils::Unused{args...};
    return nullptr;
  }
};

} // namespace Containers
} // namespace Core