#ifndef _lt_networking_tcp_Socket_hpp_
#define _lt_networking_tcp_Socket_hpp_

#include "Core/Containers/ContiguousCollection.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Core/SharedPointer.hpp"
#include "Core/Threading/Poll.hpp"
#include "Core/Threading/Signal.hpp"
#include "Modules/Networking/TCP/V4.hpp"
#include "Utils/Patterns/Prototype.hpp"
#include <Core/Containers/ContiguousCollection.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Object.hpp>
#include <Core/SharedPointer.hpp>
#include <Core/Threading/Signal.hpp>
#include <Modules/Networking/IP/V4.hpp>
#include <Modules/Networking/TCP/BasicContext.hpp>
#include <Modules/Networking/TCP/V4.hpp>
#include <Utils/Patterns/Creator.hpp>
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <format>
#include <functional>
#include <future>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace Core::Containers;
using namespace Core;
using namespace Core::Threading;

namespace Modules::Networking::TCP {

namespace {
inline static int opened_sockets_count = 0;
inline static int closed_sockets_count = 0;
} // namespace

using namespace Core::Containers;
using namespace Core::Threading;
using Logger = Core::Logging::LoggerManager;

struct Socket : public Object, public Utils::Patterns::Prototype<Socket> {

  Signal<SharedPointer<Socket>> on_disconnected;
  Signal<SharedPointer<Socket>, String> on_error;
  Signal<SharedPointer<Socket>> on_connected;
  Signal<SharedPointer<Socket>> on_closed;
  Signal<SharedPointer<Socket>> on_listening;
  Signal<SharedPointer<Socket>, SharedPointer<Socket>> on_accept;
  Signal<SharedPointer<Socket>, SharedPointer<ContiguousCollection<uint8_t>>>
      on_data;

  String name;
  int _fd;
  char buffer[2048];
  bool _close;
  bool closed = false;
  Collection<SharedPointer<Socket>> clients;

  Socket() : Socket(::socket(AF_INET, SOCK_STREAM, 0)) {}

  Socket(int fd) : _fd(fd), name(std::format("(socket fd: {})", fd)) {

    if (_fd < 0) {
      throw Core::Exceptions::RuntimeException(
          "{}: Failed to create socket: {}", name, strerror(errno));
    }

    on_error += [](auto sock, auto str) { sock->close(); };
    on_error += [](auto sock, auto str) {
      Logger::error("{}: Error: {}", sock->name.c_str(), str.c_str());
    };

    on_disconnected += [](auto sock) { sock->close(); };
  }

  virtual ~Socket() {

    if (!closed) {
      ::close(_fd);
    }

    if (read_poll != nullptr && read_poll->joinable()) {
      read_poll->join();
    }
  }

  void close() {

    if (closed) {
      return;
    }

    closed = true;

    if (accept_poll != nullptr) {
      accept_poll->quit();

      if (accept_poll->joinable()) {
        accept_poll->join();
      }
    }

    if (read_poll != nullptr) {
      read_poll->quit();
      if (read_poll->joinable()) {
        read_poll->join();
      }
    }

    ::close(_fd);

    on_closed(shared_from_this());
  }

  void listen(int port = 3000, const String &host = "0.0.0.0") {

    sockaddr_in bind_addr = Modules::Networking::TCP::V4(host, port);

    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0) {
      throw Core::Exceptions::RuntimeException("{}: Failed to listen: {}",
                                               name.c_str(), strerror(errno));
    }

    if (reuseaddr_set(true) < 0) {
      throw Core::Exceptions::RuntimeException("{}: Failed to listen: {}",
                                               name.c_str(), strerror(errno));
    }

    if (::bind(_fd, (sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
      throw Core::Exceptions::RuntimeException("{}: Failed to listen: {}",
                                               name.c_str(), strerror(errno));
    }

    if (::listen(_fd, std::thread::hardware_concurrency())) {
      throw Core::Exceptions::RuntimeException("{}: Failed to listen: {}",
                                               name.c_str(), strerror(errno));
    }

    name = std::format("(server fd: {})", _fd);

    if (accept_poll == nullptr) {
      accept_poll = Poll::create(std::bind(&Socket::accept_handle, clone()));
    }

    on_listening(shared_from_this());

    accept_poll->join();
  }

  void connect(int port, const String &host) {

    if (_fd < 0) {
      _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    }

    auto addr = Modules::Networking::TCP::V4(host, port);

    int result =
        ::connect(_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if (result < 0) {
      on_error(shared_from_this(),
               std::format("Failed to connect: {}", strerror(errno)));
      return;
    }

    on_connected(shared_from_this());

    read_start();
  }

  virtual int reuseaddr_set(bool value) {
    int opt = value;
    return setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
  }

  virtual int priotity_set(int value) {
    return setsockopt(_fd, SOL_SOCKET, SO_PRIORITY, &value, sizeof(value));
  }

  virtual unsigned int priotity_get() {
    unsigned int value;
    unsigned int size;
    int retval = getsockopt(_fd, SOL_SOCKET, SO_PRIORITY, &value, &size);
    return value;
  }

  virtual int linger_set(int value) {
    return setsockopt(_fd, SOL_SOCKET, SO_LINGER, &value, sizeof(value));
  }

  virtual int rcvtimeout_set(int value) {
    return setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &value, sizeof(value));
  }

  virtual unsigned int rcvtimeout_get() {
    unsigned value = 0;
    getsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&value, &value);
    return value;
  }

  virtual int sndtimeout_set(unsigned int value) {
    return setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &value, sizeof(value));
  }

  virtual unsigned int sndtimeout_get() {
    unsigned value = 0;
    getsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&value, &value);
    return value;
  }

  virtual int rcvbuf_set(int size) {
    return setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
  }

  virtual int recv_buffer_size_set(int size) { return 0; }

  virtual int sndbuf_set(int size) {
    return setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
  }

  virtual int send_buffer_size_max_set(int size) { return 0; }

  template <typename... ArgsTypes>
  void write(const std::format_string<ArgsTypes...> &fmt, ArgsTypes &&...args) {
    return write_string(std::format(fmt, std::forward<ArgsTypes>(args)...));
  }

  void write_string(const String &str) {
    return write_buffer(ContiguousCollection<uint8_t>(str.begin(), str.end()));
  }

  void write_buffer(const ContiguousCollection<uint8_t> &data) {
    // Try sending until all sent or an error occurs

    size_t offset = 0;

    while (offset < data.size()) {
      Logger::debug("{}: Write offset {} from {} bytes", name.c_str(), offset,
                    data.size());
      ssize_t n = ::send(_fd, data.data() + offset, data.size() - offset,
                         MSG_NOSIGNAL // no DONTWAIT
      );

      if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          // Wait until socket becomes writable again
          // (alternatively use poll/select/epoll)
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
          continue;
        }

        // Fatal send error → abort connection
        _close = true;
        break;
      }

      offset += size_t(n);
    }
  }

private:
  SharedPointer<Poll> accept_poll;
  SharedPointer<Poll> read_poll;

  void read_start() {
    if (read_poll == nullptr) {
      read_poll =
          Poll::create(std::bind(&Socket::read_handle, shared_from_this()));
    }
  }

  void accept_handle() {

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_fd, &readfds);

    timeval tv{0, 100000};

    int activity = ::select(_fd + 1, &readfds, NULL, NULL, &tv);

    if (activity < 0) {
      on_error(shared_from_this(), strerror(errno));
      return;
    }

    if (activity == 0) {
      return;
    }

    if (!FD_ISSET(_fd, &readfds)) {
      return;
    }

    // Accept client
    sockaddr_in client_addr{};
    socklen_t addrlen = sizeof(client_addr);

    int client_fd = ::accept(_fd, (sockaddr *)&client_addr, &addrlen);

    if (client_fd < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return;
      on_error(shared_from_this(), strerror(errno));
    }

    // max clients should be ensured here

    // Wrap socket
    auto conn = SharedPointer<Socket>::make(client_fd);

    conn->on_closed += [self = shared_from_this()](auto client) {
      auto lock = self->clients.acquire_lock();
      Logger::debug("{}: Removing: {}", self->name.c_str(),
                    client->name.c_str());
      auto _ = std::remove_if(
          self->clients.begin(), self->clients.end(),
          [client](auto connection) { return connection == client; });
    };

    {
      clients.acquire_lock();
      clients.push_back(conn);
    }

    auto ready = on_accept(this->clone(), conn);

    for (auto promise : ready) {
      auto future = promise->get_future();

      auto result = future.wait_for(std::chrono::seconds(5));

      if (result == std::future_status::timeout) {
        throw Core::Exceptions::RuntimeException(
            "Failed to configure socket before 5 seconds");
      }
    }

    conn->read_start();
  }

  void read_handle() {

    Logger::info("reading from fd = {}", _fd);

    fd_set read_set;
    fd_set err_set;

    FD_ZERO(&read_set);
    FD_SET(_fd, &read_set);

    FD_ZERO(&err_set);
    FD_SET(_fd, &err_set);

    timeval tv{0, 100000};

    int activity = select(_fd + 1, &read_set, NULL, &err_set, &tv);

    if (activity < 0) {
      on_error(shared_from_this(), strerror(errno));
    }

    if (activity == 0) {
      return;
    }

    if (FD_ISSET(_fd, &err_set)) {
      on_error(shared_from_this(), strerror(errno));
    }

    if (!FD_ISSET(_fd, &read_set)) {
      on_error(shared_from_this(), strerror(errno));
    }

    int read_bytes = recv(_fd, buffer, sizeof(buffer), MSG_NOSIGNAL);

    if (read_bytes < -1) {
      on_error(shared_from_this(), strerror(errno));
      return;
    }

    if (read_bytes == 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        on_error(shared_from_this(), strerror(errno));
        return;
      }

      on_disconnected(shared_from_this());
    }
    SharedPointer<ContiguousCollection<uint8_t>> chunk = nullptr;
    try {
      chunk = SharedPointer<ContiguousCollection<uint8_t>>::make(
          buffer, buffer + read_bytes);
    } catch (...) {
    }
    if (chunk != nullptr) {
      on_data(this->clone(), chunk);
    }
  }
};

} // namespace Modules::Networking::TCP

#endif