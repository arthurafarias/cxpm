#pragma once

#include "Modules/Networking/TCP/Service.hpp"
#include <string>
#include <tuple>

#include <Utils/Unused.hpp>

namespace Modules::Networking::TCP {
class Listener {
public:
  enum class ListenResultStatus { Success, Failure };
  using ListenResult = std::tuple<ListenResultStatus, std::string>;
  virtual ListenResult listen(int port, std::string host) {
    UNUSED(port);
    UNUSED(host);
    return {ListenResultStatus::Success, ""};
  }

  Listener& service_add(const Modules::Networking::TCP::Service& service) {
    UNUSED(service);
    return  *this;
  }

  
};
} // namespace Modules::Networking::TCP