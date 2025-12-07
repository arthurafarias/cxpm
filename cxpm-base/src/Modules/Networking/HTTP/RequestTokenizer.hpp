#pragma once

#include <deque>
#include <string>

namespace Modules::Networking::HTTP {


class RequestTokenizer {
public:
  std::deque<String> tokenize(String raw) {
    return std::deque<String>();
  }

private:
};
} // namespace Modules::Networking::HTTP