#pragma once

#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Networking/HTTP/Response.hpp"
#include <functional>
namespace Modules::Networking::HTTP {
class Route {
public:
  using CallbackType = std::function<void(const Request &, Response &)>;

  Method method;
  Version version;
  std::string path;
  CallbackType callback;
};
} // namespace Modules::Networking::HTTP