#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Networking/HTTP/Route.hpp"
#include "Modules/Networking/TCP/Server.hpp"
#include "Modules/Networking/TCP/Socket.hpp"
#include <exception>
#include <future>
#include <initializer_list>
#include <tuple>

namespace Modules::Networking::HTTP {

class Service : public Object,
                public EnableSharedFromThis<Service>,
                public Creator<Service> {
public:
  Signal<SharedPointer<Service>, SharedPointer<Request>,
         SharedPointer<Response>>
      on_request;

  Service() {}
  Service(const std::initializer_list<SharedPointer<Route>> &list)
      : routes(list.begin(), list.end()) {}

  SharedPointer<Service> route_add(const Method &method, String path,
                                   const Route::CallbackType &callback,
                                   const Version &version = {1, 1}) {
    route_add(Route::create(method, path, callback, version));
    return shared_from_this();
  }

  SharedPointer<Service> route_add(SharedPointer<Route> route) {
    routes.push_back(route);
    return shared_from_this();
  }

  SharedPointer<Service> listener_add(int port, const String &host) {
    auto lock = listeners.acquire_lock();
    listeners.emplace_back(port, host);
    return shared_from_this();
  }

  enum class RunResultStatus { Success, Failure };
  using RunResult = std::tuple<RunResultStatus, String>;

  RunResult run() {
    auto lock = acquire_lock();

    using PromiseType = SharedPointer<std::promise<int>>;
    using ServerType = SharedPointer<Server>;
    using ThreadType = SharedPointer<std::thread>;
    using HandlerType = std::tuple<PromiseType, ServerType, ThreadType>;

    auto handlers = Collection<HandlerType>();

    for (const auto &[port, host] : listeners) {
      PromiseType result = SharedPointer<std::promise<int>>::make();
      ServerType server = Modules::Networking::TCP::Server::create();

      server->listening += [](auto server) {
        // debug a acknowlegement
        std::osyncstream(std::cout) << "server: connected" << std::endl;
      };

      // Add a closed callback
      server->closed += [](auto server) {
        std::osyncstream(std::cout) << "server: disconnected" << std::endl;
      };

      // Add an error callback
      server->error += [](auto server, int error) {
        std::osyncstream(std::cout) << "server: error: " << error << std::endl;
      };

      shared_from_this()->on_request +=
          [](auto server, auto request, auto response) {
            auto route = server->match_route(request->resource);

            if (route != nullptr && route->callback != nullptr) {
              request->data = route->data;
              route->callback(request, response);
            }
          };

      // Add a client accepted callback so you can define the server
      // application
      server->client_accepted += [self = shared_from_this()](auto server,
                                                             auto client) {
        std::osyncstream(std::cout) << "server: client accepted " << std::endl;

        // For each client, define a disconnection message
        client->disconnected += [](auto client) {
          std::osyncstream(std::cout)
              // print a disconnection message
              << "server: client: disconnected" << std::endl;
        };

        // For each client, register an error callback
        client->error += [](auto client, auto error) {
          std::osyncstream(std::cout)
              // print the error
              << "server: client: error: " << error << std::endl;
        };

        // Register a data_received callback
        client->data_received += [server, self](auto client, auto data) {
          auto chunk = String(data.begin(), data.end());

          auto [result, request] = Request::parse(chunk);

          auto response = Response::create();
          response->client = client;

          self->on_request(self, request, response);
        };
      };

      ThreadType thread =
          ThreadType::make([server]() { server->listen(8080, "127.0.0.1"); });

      handlers.push_back({result, server, thread});
    }

    for (auto [result, server, thread] : handlers) {
      if (thread->joinable()) {
        thread->join();
      }
    }

    return {RunResultStatus::Failure, "Not Implemented"};
  }

private:
  enum class MatchRouteResultStatus { Success, Failure };
  using MatchRouteResult = SharedPointer<Route>;

  MatchRouteResult match_route(const String &resource) {

    auto el =
        std::find_if(routes.begin(), routes.end(), [resource](auto route) {
          try {
            auto data = route->parse(resource);
            route->data = data;
            return true;
          } catch (std::exception &ex) {
            // ignore route
          }
          return false;
        });

    if (el == routes.end()) {
      return nullptr;
    }

    return *el;
  }

  Collection<std::tuple<int, String>> listeners;
  Collection<SharedPointer<Route>> routes;
};

}; // namespace Modules::Networking::HTTP
