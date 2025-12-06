#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Modules/Networking/HTTP/Header.hpp"
#include "Modules/Networking/HTTP/HeaderElement.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/RequestDescriptor.hpp"
#include "Modules/Networking/HTTP/Version.hpp"
#include "Utils/Unused.hpp"
#include <sstream>
#include <string>

namespace Modules::Networking::HTTP {

class Request : public RequestDescriptor {
public:
  Request() {}
  enum class ParseResultStatus { Success, Failure };
  using ParseResult = std::tuple<ParseResultStatus, Request>;

  static inline ParseResult parse(std::string text) {
    UNUSED(text);

    auto result = Request();

    using namespace Core::Containers;
    auto lines = String::split(text, Collection<String>{"\r", "\n"});

    std::string method;
    std::string version;

    auto elements = String::split(lines.front(), " ");
    lines.pop_front();

    if (elements.size() != 3) {
      return {ParseResultStatus::Failure, result};
    }

    Collection<String> header_lines;

    for (auto line : lines) {

      if (line.empty()) {
        break;
      }

      header_lines.push_back(line);
    }

    lines.erase(lines.begin(), lines.begin() + header_lines.size() + 1);

    result.header = Header::parse(header_lines);

    result.body = String::join(lines, "\n");

    return {ParseResultStatus::Success, result};
  }
};

} // namespace Modules::Networking::HTTP