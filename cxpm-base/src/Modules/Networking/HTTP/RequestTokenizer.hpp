#pragma once

#include <deque>
#include <string>

namespace Modules::Networking::HTTP {


class RequestTokenizer {
public:
  std::deque<std::string> tokenize(std::string raw) {
    return std::deque<std::string>();
  }

private:
};
} // namespace Modules::Networking::HTTP