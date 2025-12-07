#pragma once

#include <Core/Containers/String.hpp>
#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>
using namespace Core::Containers;

namespace Modules::Networking::IP {

template <typename type>
inline uint32_t V4(type a3, type a2, type a1, type a0) {
  return htonl((a3 << 24) | (a2 << 16) | (a1 << 8) | (a0 << 0));
}

inline uint32_t V4(const String &addr) {
  uint32_t _addr;
  inet_pton(AF_INET, addr.c_str(), &_addr);
  return _addr;
}

}; // namespace Modules::Networking::IP