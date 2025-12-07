#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Variant.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Networking/HTTP/Route.hpp"
#include "Modules/Networking/TCP/Server.hpp"
#include "Modules/Networking/TCP/Socket.hpp"
#include "Utils/Unused.hpp"
#include <future>
#include <memory>
#include <tuple>

namespace Modules::Networking::HTTP {

class Service : public Object {
public:
  Service &route_add(const Method &method, String path,
                     const Route::CallbackType &callback,
                     const Version &version = {1, 1}) {
    return *this;
  }

  Service &add_listener(int port, const String &host) {
    auto lock = listeners.acquire_lock();
    listeners.emplace_back(port, host);
    return *this;
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

      // server->listening += [](auto server) {
      //   // debug a acknowlegement
      //   std::osyncstream(std::cout) << "server: connected" << std::endl;
      // };

      // server->client_accepted += [](auto sender, SharedPointer<Socket>
      // client) {
      //   std::osyncstream(std::cout) << "server: client connected" <<
      //   std::endl;

      //   client->data_received += [](auto sender, auto data) {
      //     Logging::LoggerManager::info("Data:\n{}\n", data);
      //   };
      // };

      // ThreadType thread =
      //     SharedPointer<std::thread>::make([result, server, port, host]() {
      //       auto _result = server->listen(port, host.c_str());
      //       result->set_value(_result);
      //     });
      // handlers.push_back({result, server, thread});

      // Add a listening callback

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

      // Add a client accepted callback so you can define the server application
      server->client_accepted += [](auto server, auto client) {
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
        client->data_received += [](auto client, auto data) {
          // Print the data received from the client
          std::osyncstream(std::cout)
              << "server: client: data: "
              << std::string(data.begin(), data.end()) << std::endl;

          // echo behaviour: write back to the client the data received
          client->write(data);
        };
      };

      ThreadType thread =
          ThreadType::make([server]() { server->listen(8080, "127.0.0.1"); });

      handlers.push_back({result, server, thread});
    }

    for (auto [result, server, thread] : handlers) {
      // auto future = result->get_future();
      // future.wait();
      if (thread->joinable()) {
        thread->join();
      }
    }

    return {RunResultStatus::Failure, "Not Implemented"};
  }

private:
  Collection<std::tuple<int, String>> listeners;
};

// class Service : public Modules::Networking::TCP::Server, public Object {
// public:

//   void route_add(const Method &method, String path,
//                  const Route::CallbackType &callback,
//                  const Version &version = {1, 1}) {

//     auto lk = routes.acquire_lock();
//     routes.emplace(path, Route{method, version, path, callback});
//   }

//   void route_add(const Route &route) {
//     auto lk = routes.acquire_lock();
//     routes.emplace(route.model, route);
//     UNUSED(route);
//   }

//   enum class RunResultStatus { Success, Failure };
//   using RunResult = std::tuple<RunResultStatus, String>;

//   RunResult run() {

//     auto promise_collection =
//     Collection<SharedPointer<std::promise<bool>>>();

//     for (const auto &[port, host] : listeners) {

//       auto server = Server::create();

//       auto quit_promise = SharedPointer<std::promise<bool>>::make();

//       socket->on_error +=
//           [](auto) { Logging::LoggerManager::info("Server Error"); };
//       socket->on_client_accepted += [](auto server, auto client) {
//         Logging::LoggerManager::info("Client Accepted");
//       };
//       socket->on_connect +=
//           [](auto) { Logging::LoggerManager::info("Server Connected"); };
//       socket->on_listening +=
//           [](auto) { Logging::LoggerManager::info("Server Listening"); };
//       socket->on_start_accepting +=
//           [](auto) { Logging::LoggerManager::info("Server Start Accepting");
//           };

//       socket->on_disconnect += [quit_promise, &descriptor, this](auto) {
//         Logging::LoggerManager::info("Server Disconnected");
//         auto lock = listeners.acquire_lock();
//         auto pos = std::find(listeners.begin(), listeners.end(), descriptor);
//         listeners.erase(pos);
//         quit_promise->set_value(true);
//       };

//       socket->on_data_received +=
//           [descriptor, this](SharedPointer<Socket> socket, Buffer buffer) {
//             Logging::LoggerManager::info("Server Data Received");

//             auto lock = listeners.acquire_lock();
//             auto [result, request] =
//                 HTTP::Request::parse(String(buffer.begin(), buffer.end()));

//             if (result == Request::ParseResultStatus::Failure) {
//               // TODO: don't ignore later
//               return;
//             }

//             if (routes.contains(request.resource)) {
//               {
//                 auto response = Response{.socket = socket};
//                 auto route = routes[request.resource];
//                 route.callback(request, response);
//               }
//             }
//           };

//       promise_collection.push_back(quit_promise);
//     }

//     for (auto quit_promise : promise_collection) {
//       auto future = quit_promise->get_future();
//       future.wait();
//       future.get();
//     }

//     return {RunResultStatus::Failure, "Not Impemented"};
//   }

// private:
//   Map<String, Route> routes;
//   Collection<std::tuple<int, String>> listeners;
// };
}; // namespace Modules::Networking::HTTP
