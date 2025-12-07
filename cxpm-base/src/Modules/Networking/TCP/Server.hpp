#ifndef _lt_networking_tcp_Server_hpp_
#define _lt_networking_tcp_Server_hpp_

#include "Core/SharedPointer.hpp"
#include "Modules/Networking/TCP/BasicContext.hpp"
#include "Utils/Patterns/Creator.hpp"

#include <Core/Object.hpp>

#include <Modules/Networking/IP/V4.hpp>
#include <Modules/Networking/TCP/BasicContext.hpp>
#include <Modules/Networking/TCP/Client.hpp>
#include <Modules/Networking/TCP/Socket.hpp>
#include <Modules/Networking/TCP/V4.hpp>

#include <Core/Threading/Signal.hpp>

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <cstring>
#include <memory>
#include <mutex>
#include <sys/select.h>
#include <unistd.h>
#include <vector>

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

namespace Modules::Networking::TCP {

using namespace Core;
using namespace Core::Threading;

class Server : public Object,
               public std::enable_shared_from_this<Server>,
               public Utils::Patterns::Creator<Server> {
public:
  Signal<SharedPointer<Server>, int> error;
  Signal<SharedPointer<Server>> listening;
  Signal<SharedPointer<Server>> closed;

  Signal<SharedPointer<Server>, SharedPointer<Socket>> client_accepted;
  Server(SharedPointer<BasicContext> context = nullptr) : _context(context) {
    client_accepted +=
        [this](auto Server, auto client) { handle_accept(Server, client); };
  }

  virtual ~Server() { close(); }
  virtual int listen(int port, const char *host = "0.0.0.0",
                     size_t max_connections = 1024,
                     size_t recv_buffer_size = 16384,
                     size_t send_buffer_size = 16384) {
    if (_socket != nullptr)
      return -EALREADY;

    sockaddr_in bind_addr = TCP::V4(host, port);

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
      return -errno;

    _socket = Socket::create(_context, fd);
    if (!_socket)
      return -ENOMEM;

    // socket options
    if (_socket->reuseaddr_set(true) < 0)
      return -errno;

    if (_socket->rcvbuf_set(recv_buffer_size) < 0)
      return -errno;

    if (_socket->sndbuf_set(send_buffer_size) < 0)
      return -errno;

    // bind
    if (::bind(fd, (sockaddr *)&bind_addr, sizeof(bind_addr)) < 0)
      return -errno;

    // listen
    if (::listen(fd, (int)max_connections) < 0)
      return -errno;

    // notify user
    listening(shared_from_this());

    // accept loop
    while (!_socket->_close) {
      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(fd, &readfds);

      timeval tv{1, 0};

      int activity = ::select(fd + 1, &readfds, NULL, NULL, &tv);
      if (activity < 0)
        break;

      if (activity == 0)
        continue;

      if (!FD_ISSET(fd, &readfds))
        continue;

      // Accept client
      sockaddr_in client_addr{};
      socklen_t addrlen = sizeof(client_addr);

      int client_fd = ::accept(fd, (sockaddr *)&client_addr, &addrlen);

      if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
          continue;
        return -errno;
      }

      // Wrap socket
      auto conn = SharedPointer<Socket>::make(_socket->_context, client_fd);

      conn->sndbuf_set(send_buffer_size);
      conn->rcvbuf_set(recv_buffer_size);

      // cleanup lambda
      conn->disconnected += [this, conn](auto) {
        auto lock = acquire_lock();
        _clients.erase(std::remove_if(_clients.begin(), _clients.end(),
                                      [&](auto &c) { return c == conn; }),
                       _clients.end());
      };

      // notify user
      client_accepted(shared_from_this(), conn);
    }

    close();
    return 0;
  }

  virtual void close() {
    _socket->close();
    for (auto client : _clients) {
      client->close();
    }
  }

protected:
  SharedPointer<BasicContext> _context;
  SharedPointer<Socket> _socket;
  std::vector<SharedPointer<Socket>> _clients;

  void handle_accept(SharedPointer<Server>, SharedPointer<Socket> client) {
    client->start();
  }
};
} // namespace Modules::Networking::TCP

#endif