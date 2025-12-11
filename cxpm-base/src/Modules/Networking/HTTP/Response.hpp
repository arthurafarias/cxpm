#pragma once

#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/ResponseStatus.hpp"
#include "Modules/Networking/HTTP/Version.hpp"
#include "Modules/Networking/TCP/Client.hpp"
#include "Modules/Networking/TCP/Socket.hpp"
#include "Modules/Streams/OutputStream.hpp"
#include "Utils/Unused.hpp"
#include <atomic>
#include <chrono>
#include <ostream>
#include <syncstream>

using namespace Modules::Networking::TCP;

namespace Modules::Networking::HTTP {

class ResponseDescriptor {
public:
  ResponseStatus status;
  Version version = {1, 0};

  Map<String, String> headers = {
      {"Date",
       std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now())},
      {"Content-Type", std::format("{}", "text/html; charset=UTF-8")},
      {"Expires", std::format("{}", "-1")}};

  String body;
};

inline std::ostream &operator<<(OutputStream &os,
                                const ResponseDescriptor &res) {
  os << std::format("HTTP/{}.{} {} {}\n", res.version.major,
                    res.version.minor, res.status.code,
                    res.status.description.c_str());
  for (auto header : res.headers) {
    os << std::format("{}: {}\n", header.first.c_str(), header.second.c_str());
  }
  os << std::format("\n{}\n\n", res.body.c_str());

  return os;
}

class Response;

class HTTPClientConnection
    : public Object,
      public EnableSharedFromThis<HTTPClientConnection>,
      public Utils::Patterns::Creator<HTTPClientConnection> {
public:
  HTTPClientConnection(SharedPointer<AbstractSocket> socket) : socket(socket), sent(false) {}
  SharedPointer<AbstractSocket> socket;
  std::atomic_bool sent = false;

  void close() { socket->close(); }

  void send(const String &response) {
    auto lock = acquire_lock();
    socket->write(response);
    sent = true;
  }
};

class Response : public ResponseDescriptor,
                 public Object,
                 public EnableSharedFromThis<Response>,
                 public Utils::Patterns::Creator<Response> {
public:
  SharedPointer<HTTPClientConnection> client;
  void send(bool close = true) {
    auto lock = acquire_lock();
    auto ss = std::stringstream();
    std::osyncstream(ss) << static_cast<ResponseDescriptor>(*this);

    if (client->sent == true) {
      throw Core::Exceptions::RuntimeException("Response already sent");
    }

    client->send(ss.str());

    if (close == true) {
      client->close();
    }
  }
};

} // namespace Modules::Networking::HTTP