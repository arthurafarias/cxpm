#pragma once

#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/ResponseStatus.hpp"
#include "Modules/Networking/HTTP/Version.hpp"
#include "Modules/Streams/OutputStream.hpp"
#include "Utils/Patterns/Creator.hpp"
#include <chrono>
#include <ostream>
#include <sstream>

namespace Modules::Networking::HTTP {

class ResponseDescriptor {
public:
  ResponseStatus status;
  Version version = {1, 0};

  Map<String, String> headers = {
      {"Date",
       String::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now())},
      {"Content-Type", String::format("{}", "text/html; charset=UTF-8")},
      {"Expires", String::format("{}", "-1")}};

  String body;
};

inline std::ostream &operator<<(OutputStream &os,
                                const ResponseDescriptor &res) {
  os << String::format("HTTP/{}.{} {} {}\n", res.version.major, res.version.minor,
                    res.status.code, res.status.description.c_str());
  for (auto header : res.headers) {
    os << String::format("{}: {}\n", header.first.c_str(), header.second.c_str());
  }
  os << String::format("\n{}\n\n", res.body.c_str());

  return os;
}

class Response;

class Response : public ResponseDescriptor,
                 public Core::Object,
                 public EnableSharedFromThis<Response>,
                 public Utils::Patterns::Creator<Response> {
public:
  String to_string() {
    auto os = std::ostringstream();
    os << static_cast<ResponseDescriptor>(*this);
    return os.str();
  }
};

} // namespace Modules::Networking::HTTP