#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Core/SharedPointer.hpp"
#include "Core/Threading/ThreadPool.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Networking/HTTP/Response.hpp"
#include "Modules/Networking/HTTP/Route.hpp"
#include "Modules/Networking/TCP/Socket.hpp"
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
  Signal<SharedPointer<Service>, SharedPointer<Socket>,
         SharedPointer<Request>, SharedPointer<Response>>
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
    auto lock = routes.acquire_lock();
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
    using ServerType = SharedPointer<Socket>;
    using ThreadType = SharedPointer<std::thread>;
    using HandlerType = std::tuple<PromiseType, ServerType, ThreadType>;

    auto handlers = Collection<HandlerType>();

    for (const auto &[port, host] : listeners) {
      PromiseType result = SharedPointer<std::promise<int>>::make();
      ServerType server = Modules::Networking::TCP::Socket::create();

      server->on_listening += [](auto server) {
        // debug a acknowlegement
        Logging::LoggerManager::info("Connected");
      };

      // Add a closed callback
      server->on_closed +=
          [](auto server) { Logging::LoggerManager::info("Disconnected"); };

      // Add an error callback
      server->on_error += [](auto server, auto error) {
        Logging::LoggerManager::info("Error {}", error);
      };

      shared_from_this()->on_request +=
          [](auto server, auto client, auto request, auto response) {
            auto lock = response->acquire_lock();

            auto route = server->match_route(request);

            if (route != nullptr && route->callback != nullptr) {
              request->data = route->data;
              ThreadPool::get_instance().submit(
                  [route, request, response, client]() {
                    auto lock = route->acquire_lock();
                    route->callback(client, request, response);
                  });
              return;
            }

            response->status = {404, "Not Found"};
            response->headers["Connection"] = "Close";
            // client->write(response->to_string(), true);
            client->close();
          };

      // Add a client accepted callback so you can define the server
      // application
      server->on_accept +=
          [self = shared_from_this()](auto server, auto client) {
            auto lock = client->acquire_lock();

            Logging::LoggerManager::info("Client Accepted");

            // For each client, define a disconnection message
            client->on_disconnected += [](auto client) {
              Logging::LoggerManager::info("Client Disconnected");
            };

            // For each client, register an error callback
            client->on_error += [](auto client, auto error) {
              Logging::LoggerManager::info("Client Error");
            };

            // Register a data_received callback
            client->on_data += [server, self](auto client, auto data) {
              auto response = Response::create();
              response->status = {200, "OK"};

              auto chunk = String(data->begin(), data->end());
              auto [result, request] = Request::parse(chunk);

              if (result == Request::ParseResultStatus::Failure) {
                response->status = ResponseStatus{400, "Invalid Request"};
                response->headers["Connection"] = "Close";
                // client->write(response->to_string());
                return;
              }

              self->on_request(self, client, request, response);
            };
          };

      ThreadType thread = ThreadType::make(
          [server, &host, &port]() { server->listen(port, host.c_str()); });
      {
        auto lock = handlers.acquire_lock();
        handlers.push_back({result, server, thread});
      }
    }

    {
      auto lock = handlers.acquire_lock();
      for (auto [result, server, thread] : handlers) {

        if (thread->joinable()) {
          thread->join();
        }
      }
    }

    return {RunResultStatus::Failure, "Not Implemented"};
  }

private:
  enum class MatchRouteResultStatus { Success, Failure };
  using MatchRouteResult = SharedPointer<Route>;

  MatchRouteResult match_route(const SharedPointer<Request> request) {
    auto lock1 = routes.acquire_lock();

    auto el = std::find_if(routes.begin(), routes.end(), [request](auto route) {
      try {
        request->acquire_lock();
        auto lock = route->acquire_lock();
        auto data = route->parse(request->resource);
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

    {
      request->acquire_lock();
      if ((*el)->method != request->method) {
        return nullptr;
      }
    }

    return *el;
  }

  Collection<std::tuple<int, String>> listeners;
  Collection<SharedPointer<Route>> routes;
};

}; // namespace Modules::Networking::HTTP
