#pragma once

#include "Core/Exceptions/RuntimeException.hpp"
#include "Modules/Streams/OutputStream.hpp"
#include "Modules/Streams/StreamBuffer.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <ostream>
#include <streambuf>
#include <sys/socket.h>
#include <unistd.h>

#include <Core/Object.hpp>
#include <Utils/Macros/MaybeUnused.hpp>
using namespace Core;

namespace Modules::Networking::TCP {
// Custom stream buffer that writes to a socket FD
class SocketStreamBuffer : public StreamBuffer, public Object {
private:
  int sockfd;

protected:
  // Override overflow to write characters to the socket
  int overflow(MaybeUnused int c = traits_type::eof()) override {
    auto lk = acquire_lock();
    if (c != EOF) {
      char ch = static_cast<char>(c);
      ssize_t n = ::send(sockfd, &ch, 1, 0);
      if (n == 1)
        return c; // success
      return EOF;
      // throw Exceptions::RuntimeException("Failed to send {}",
      // ::strerror(errno));
    }
    return EOF;
  }

public:
  explicit SocketStreamBuffer(int fd) : sockfd(fd) {}
};

// Custom ostream that uses the socket stream buffer
class SocketStream : public OutputStream {
private:
  SocketStreamBuffer buf;

public:
  explicit SocketStream(int fd) : OutputStream(&buf), buf(fd) {}
};

} // namespace Modules::Networking::TCP