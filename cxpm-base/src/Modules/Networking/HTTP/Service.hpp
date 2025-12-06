#pragma once

#include "Core/Containers/Collection.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Route.hpp"
#include "Utils/Unused.hpp"
#include <Modules/Networking/TCP/Service.hpp>

namespace Modules::Networking::HTTP {
class Service : public Modules::Networking::TCP::Service {
public:
  void route_add(const Method &method, std::string path,
                 const Route::CallbackType &callback,
                 const Version &version = {1, 1}) {
    UNUSED(method);
    UNUSED(path);
    UNUSED(callback);
    UNUSED(version);
    auto lk = routes.acquire_lock();
  }

  void route_add(const Route &route) {
    auto lk = routes.acquire_lock();
    UNUSED(route);
  }

private:
  Core::Containers::Collection<Route> routes;
};
}; // namespace Modules::Networking::HTTP