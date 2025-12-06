#pragma once

#include "Modules/Networking/HTTP/ResponseStatus.hpp"
#include "Modules/Networking/HTTP/Version.hpp"
#include "Modules/Networking/TCP/Socket.hpp"
#include "Utils/Unused.hpp"
#include <chrono>

using namespace Modules::Networking::TCP;

namespace Modules::Networking::HTTP {

class ResponseDescriptor {
public:
  ResponseDescriptor() {}
  ResponseStatus status = ResponseStatuses[200];
  Version version = {1, 1};

  Map<String, String> headers = {
      {"Date", std::format("{}", std::chrono::system_clock::now())},
      {"Content-Type", std::format("{}", "text/html; charset=UTF-8")},
      {"Expires", std::format("{}", "-1")}};

  String body;
};

class Response : public ResponseDescriptor {
public:
  Socket socket;

  void send(int status_code = 200) {

    socket << std::format("HTTP/{}.{} {} {}\n", version.major, version.minor,
                          status.code, status.description);

    for (auto header : headers) {
      socket << std::format("{}: {}\n\n", header.first, header.second);
    }

    socket << std::format("{}", body);

    UNUSED(status_code);
  }
};

} // namespace Modules::Networking::HTTP