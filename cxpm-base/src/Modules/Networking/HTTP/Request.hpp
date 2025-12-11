#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/Header.hpp"
#include "Modules/Networking/HTTP/HeaderElement.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/RequestDescriptor.hpp"
#include "Modules/Networking/HTTP/Version.hpp"
#include "Modules/Testing/TestCase.hpp"
#include "Utils/Patterns/Creator.hpp"
#include "Utils/Unused.hpp"
#include <sstream>
#include <string>

namespace Modules::Networking::HTTP {

class Request : public RequestDescriptor,
                public Object,
                public EnableSharedFromThis<Request>,
                Utils::Patterns::Creator<Request> {
public:
  Request() {}
  enum class ParseResultStatus { Success, Failure };
  using ParseResult = std::tuple<ParseResultStatus, SharedPointer<Request>>;

  static inline ParseResult parse(String text) {
    UNUSED(text);

    auto result = create();

    using namespace Core::Containers;
    auto _ = std::remove_if(text.begin(), text.end(),
                            [](auto c) { return c == '\r'; });
    auto lines = String::split(text, "\n");

    auto elements = String::split(lines.front(), " ");
    lines.pop_front();

    if (elements.size() != 3) {
      return {ParseResultStatus::Failure, result};
    }

    if (!MethodReverseMap.contains(elements[0])) {
      return {ParseResultStatus::Failure, result};
    }

    result->method = MethodReverseMap.at(elements[0]);
    result->resource = elements[1];
    result->version = Version(elements[2]);

    Collection<String> header_lines;

    for (auto line : lines) {

      if (line.empty()) {
        break;
      }

      header_lines.push_back(line);
    }

    if (lines.size() > 0 && header_lines.size() > 0) {
      lines.erase(lines.begin(), lines.begin() + header_lines.size());
    }

    result->header = Header::parse(header_lines);

    result->body = String::join(lines, "\n");

    return {ParseResultStatus::Success, result};
  }

private:
};

class RouteTest : public Modules::Testing::TestCase {
  inline static int main(int argc, char *argv[]) {

    auto model = "/:user/:session_id";
    auto url = "/arthur"; // it's not tree like

    auto compile = [](const String &model) -> Map<String, int> {
      auto context = Map<String, int>();

      auto path_collection = String::split(model, "/");

      for (size_t i = 0; i < path_collection.size(); i++) {
        if (path_collection[i].starts_with(":")) {
          context[path_collection[i].substr(1, path_collection[i].size())] = i;
        }
      }

      return context;
    };

    auto parse = [](const Map<String, int> context,
                    String url) -> Map<String, String> {
      auto result = Map<String, String>();
      auto haystack = String::split(url, "/");

      for (auto [key, value] : context) {
        if (value < haystack.size()) {
          result[key] = haystack[value];
        }
      }

      return result;
    };

    auto context = compile(model);
    auto result = parse(context, url);

    return 0;
  }
};

} // namespace Modules::Networking::HTTP