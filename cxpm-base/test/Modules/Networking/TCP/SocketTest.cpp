#include "Core/Logging/Manager.hpp"
#include <Modules/Networking/TCP/Socket.hpp>

using namespace Modules::Networking::TCP;

int main(int argc, char *argv[]) {

  Logging::Logger::level_set(Logging::Logger::Level::Debug);
  Logging::Logger::stream_set(Logging::Logger::stream_cout());

  auto server = Socket::create();

  server->on_listening +=
      [](auto server) { Logging::Logger::info("Listening"); };

  server->on_accept += [](auto server, auto client) {
    Logging::Logger::info("Client Accepted");
    client->on_data += [](auto client, auto data) {
      static int counter = 0;
      client->write("Counter: {}, FD: {}\n", counter++, client->_fd.load());
      client->close();
    };
  };

  server->on_closed +=
      [](auto server) { Logging::Logger::info("Closed"); };

  server->listen(3000, "127.0.0.1");

  return 0;
}