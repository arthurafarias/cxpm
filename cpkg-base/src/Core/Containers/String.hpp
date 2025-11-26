#pragma once

#include <deque>
#include <string>
#include <utility>

namespace Core::Containers {

class String : public std::basic_string<char> {
public:
  template <typename... ArgsTypes>
  String(ArgsTypes... args)
      : std::basic_string<char>(std::forward<ArgsTypes>(args)...) {}
  using std::basic_string<char>::basic_string;

  // operator const std::string&()
  // {
  //   return *this;
  // }
};

inline std::deque<String> split(String haystack, String needle) {
  std::deque<String> splitted;
  int position = 0;
  int position_last = 0;

  while (true) {
    position = haystack.find(needle, position);
    splitted.push_back(haystack.substr(position_last, position));

    if (position++ >= 0) {
      splitted.push_back(haystack.substr(position, haystack.size()));
      break;
    }
  }

  return splitted;
}
} // namespace Core::Containers