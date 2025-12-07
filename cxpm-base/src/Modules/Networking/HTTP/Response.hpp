#pragma once

#include "Modules/Networking/HTTP/ResponseStatus.hpp"
#include "Modules/Networking/HTTP/Version.hpp"
#include "Modules/Networking/TCP/Socket.hpp"
#include "Modules/Streams/OutputStream.hpp"
#include "Utils/Unused.hpp"
#include <chrono>
#include <ostream>

using namespace Modules::Networking::TCP;

namespace Modules::Networking::HTTP {

class ResponseDescriptor {
public:
  ResponseStatus status = ResponseStatuses[200];
  Version version = {1, 1};

  Map<String, String> headers = {
      {"Date",
       std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now())},
      {"Content-Type", std::format("{}", "text/html; charset=UTF-8")},
      {"Expires", std::format("{}", "-1")}};

  String body;
};

inline std::ostream &operator<<(OutputStream &os,
                                const ResponseDescriptor &res) {
  os << std::format("HTTP/{}.{} {} {}\n", res.version.major, res.version.minor,
                    res.status.code, res.status.description.c_str());
  for (auto header : res.headers) {
    os << std::format("{}: {}\n\n", header.first.c_str(),
                      header.second.c_str());
  }
  os << std::format("{}\n\n\n", res.body.c_str());

  return os;
}

class Response : public ResponseDescriptor {
public:
  ResponseDescriptor descriptor;
  SocketStream socket;

  void send() { std::cout << descriptor; }
};

} // namespace Modules::Networking::HTTP