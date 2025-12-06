#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Object.hpp"
#include "Utils/Unused.hpp"

using namespace Core;
using namespace Core::Containers;

namespace Modules::Networking::TCP {
class Socket : public Object {};

static inline Socket &operator<<(Socket &socket, const String &string) {
  UNUSED(socket);
  UNUSED(string);
  return socket;
}
} // namespace Modules::Networking::TCP