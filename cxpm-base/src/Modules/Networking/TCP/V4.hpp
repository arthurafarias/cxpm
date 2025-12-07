#ifndef _lt_networking_tcp_addr_hpp_
#define _lt_networking_tcp_addr_hpp_

#include <Core/Containers/String.hpp>
#include <Modules/Networking/IP/V4.hpp>

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <sys/select.h>
#include <unistd.h>

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

using namespace Core::Containers;
using namespace Modules::Networking::IP;

namespace Modules::Networking::TCP {

template <typename type> inline struct sockaddr_in V4(type ip, uint16_t port) {
  return {.sin_family = AF_INET,
          .sin_port = htons(port),
          .sin_addr = {.s_addr = 0}};
}

template <>
inline struct sockaddr_in V4<uint32_t>(uint32_t ip, uint16_t port) {
  return {.sin_family = AF_INET,
          .sin_port = htons(port),
          .sin_addr = {.s_addr = ip}};
}

template <>
inline struct sockaddr_in V4<const String &>(const String &ip, uint16_t port) {
  return {.sin_family = AF_INET,
          .sin_port = htons(port),
          .sin_addr = {.s_addr = IP::V4(ip)}};
}

} // namespace Modules::Networking::TCP

#endif