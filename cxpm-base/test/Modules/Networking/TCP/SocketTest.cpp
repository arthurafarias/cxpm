#include "Core/Logging/LoggerManager.hpp"
#include <Modules/Networking/TCP/Socket.hpp>

using namespace Modules::Networking::TCP;

int main(int argc, char *argv[]) {

  Logging::LoggerManager::level_set(Logging::LoggerManager::Level::Debug);
  Logging::LoggerManager::stream_set(Logging::LoggerManager::stream_cout());

  auto server = Socket::create();

  server->on_listening +=
      [](auto server) { Logging::LoggerManager::info("Listening"); };

  server->on_accept += [](auto server, auto client) {
    Logging::LoggerManager::info("Client Accepted");
    client->on_data += [](auto client, auto data) {
      static int counter = 0;
      client->write("Counter: {}, FD: {}\n", counter++, client->_fd);
      client->close();
    };
  };

  server->on_closed +=
      [](auto server) { Logging::LoggerManager::info("Closed"); };

  server->listen(3000, "127.0.0.1");

  return 0;
}