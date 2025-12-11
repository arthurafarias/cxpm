#ifndef _lt_networking_tcp_Socket_hpp_
#define _lt_networking_tcp_Socket_hpp_

#include <Core/Object.hpp>
#include <Modules/Networking/IP/V4.hpp>
#include <Modules/Networking/TCP/BasicContext.hpp>
#include <Modules/Networking/TCP/V4.hpp>

#include <Core/SharedPointer.hpp>
#include <Core/Threading/Signal.hpp>
#include <Utils/Patterns/Creator.hpp>

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <atomic>
#include <bits/types/struct_timeval.h>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <syncstream>
#include <sys/select.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

using namespace Core::Threading;

namespace Modules::Networking::TCP {

using namespace Core;

class BasicSocket {
public:
  BasicSocket(SharedPointer<BasicContext> context) : _context(context) {}

  virtual int write(const String &data) = 0;
  virtual int write(const std::vector<char> &data) = 0;
  virtual int write(const std::vector<uint8_t> &data) = 0;

  virtual void close() = 0;

protected:
  SharedPointer<BasicContext> _context;
};

class AbstractSocket : public Object,
                       public BasicSocket,
                       public EnableSharedFromThis<AbstractSocket> {
public:
  AbstractSocket(SharedPointer<BasicContext> context) : BasicSocket(context) {}
  Signal<SharedPointer<AbstractSocket>, int> error;
  Signal<SharedPointer<AbstractSocket>> connected;
  Signal<SharedPointer<AbstractSocket>> disconnected;
  Signal<SharedPointer<AbstractSocket>, std::vector<char>> data_received;
};

class Socket : public AbstractSocket, public Utils::Patterns::Creator<Socket> {
public:
  Socket(SharedPointer<BasicContext> context, int fd = -1)
      : AbstractSocket(context), _fd(fd) {
    Logging::LoggerManager::info("Socket Created fd: {}", fd);
  }

  virtual ~Socket() {

    _close = true;

    if (handle_read_thread.joinable()) {
      handle_read_thread.join();
    }

    if (handle_write_thread.joinable()) {
      handle_write_thread.join();
    }

    ::close(_fd);
  }

  int connect() {
    std::thread(&Socket::start, this).detach();
    return 0;
  }

  int connect(int port, uint32_t ip) {
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    _addr = TCP::V4(ip, port);

    int result =
        ::connect(_fd, (struct sockaddr *)&_addr, sizeof(struct sockaddr_in));

    if (result < 0) {
      return result;
    }

    return start();
  }

  int connect(int port, const String &ip = "127.0.0.1") {
    auto lock = acquire_lock();
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    _addr = TCP::V4(ip, port);

    int result =
        ::connect(_fd, (struct sockaddr *)&_addr, sizeof(struct sockaddr_in));

    if (result < 0) {
      return result;
    }

    return start();
  }

  int write(const String &data) {
    std::unique_lock<std::mutex> lk(_send_buffer_mutex);
    _send_buffer.append_range(data);
    _send_buffer_cond.notify_one();
    return data.size();
  }

  int write(const std::vector<char> &data) {
    std::unique_lock<std::mutex> lk(_send_buffer_mutex);
    _send_buffer.append_range(data);
    _send_buffer_cond.notify_one();
    return data.size();
  }

  int write(const std::vector<uint8_t> &data) {
    std::unique_lock<std::mutex> lk(_send_buffer_mutex);
    _send_buffer.append_range(data);
    _send_buffer_cond.notify_one();
    return data.size();
  }

  template <typename iterator_type>
  int write(const iterator_type &begin, const iterator_type &end) {
    std::unique_lock<std::mutex> lk(_send_buffer_mutex);
    _send_buffer.append_range(begin, end);
    _send_buffer_cond.notify_one();
    return end - begin;
  }

  void close() {

    _close = true;

    if (handle_read_thread.joinable()) {
      handle_read_thread.join();
    }

    if (handle_write_thread.joinable()) {
      handle_write_thread.join();
    }

    ::close(_fd);

    disconnected(shared_from_this());
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

  virtual int recv_buffer_size_set(int size) {
    _recv_buffer_size = size;
    return 0;
  }

  virtual int sndbuf_set(int size) {
    return setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
  }

  virtual int send_buffer_size_max_set(int size) {
    _send_buffer_size = size;
    return 0;
  }

  virtual const int &fd() { return _fd; }

protected:
  int _fd = -1;
  std::atomic_bool _started = false;
  struct sockaddr_in _addr;
  std::atomic_bool _close = false;

  std::mutex _send_buffer_mutex;
  std::condition_variable _send_buffer_cond;
  std::vector<char> _send_buffer = std::vector<char>();

  std::thread handle_connection_thread;
  std::thread handle_write_thread;
  std::thread handle_read_thread;
  std::atomic_bool _write_thread_started = false;

  size_t _send_buffer_size = 16384;
  size_t _recv_buffer_size = 16384;

  friend class Server;

  int start() {

    if (_started) {
      return 0;
    }

    _close = false;

    handle_write_thread = std::thread(std::bind(&Socket::handle_write, this));

    connected(shared_from_this());

    _started = true;
    return 0;
  }

  int handle_write() {
    handle_read_thread = std::thread(std::bind(&Socket::handle_read, this));

    while (!(_write_thread_started && _close)) {

      std::vector<char> local_buf;

      {
        // Wait for data or close
        std::unique_lock<std::mutex> lk(_send_buffer_mutex);

        _write_thread_started = true;
        auto result = _send_buffer_cond.wait_for(
            lk, std::chrono::milliseconds(50),
            [this]() { return _close || !_send_buffer.empty(); });

        // Take a copy of the buffer chunk to write
        local_buf.assign(_send_buffer.begin(), _send_buffer.end());
      }

      // Try sending until all sent or an error occurs
      size_t offset = 0;

      while (offset < local_buf.size()) {
        ssize_t n =
            ::send(_fd, local_buf.data() + offset, local_buf.size() - offset,
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

      // Remove what we sent from the shared buffer
      {
        std::unique_lock<std::mutex> lk(_send_buffer_mutex);

        if (offset > 0 && offset <= _send_buffer.size()) {
          _send_buffer.erase(_send_buffer.begin(),
                             _send_buffer.begin() + offset);
        }

        if (_send_buffer.empty())
          _send_buffer_cond.notify_all();
      }
    }

    // ::close(_fd);
    return 0;
  }

  int handle_read() {
    std::vector<char> buffer(2048);

    while (!_close) {

      fd_set read_set;
      fd_set err_set;

      FD_ZERO(&read_set);
      FD_SET(_fd, &read_set);

      FD_ZERO(&err_set);
      FD_SET(_fd, &err_set);

      timeval tv{0, 500};

      int activity = select(_fd + 1, &read_set, NULL, &err_set, &tv);
      if (activity < 0)
        break;
      if (activity == 0)
        continue;

      if (FD_ISSET(_fd, &err_set))
        break;

      if (!FD_ISSET(_fd, &read_set))
        continue;

      int read_bytes = recv(_fd, buffer.data(), buffer.size(), MSG_NOSIGNAL);

      if (read_bytes <= 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
          continue;
        break;
      }

      // copy exactly the received part
      std::vector<char> chunk(buffer.begin(), buffer.begin() + read_bytes);

      data_received(shared_from_this(), chunk);
    }

    // ::close(_fd);
    return 0;
  }
};
} // namespace Modules::Networking::TCP

#endif