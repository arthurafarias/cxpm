
#include "Core/Logging/LoggerManager.hpp"
#include "Modules/Networking/TCP/Socket.hpp"

using namespace Modules::Networking::TCP;

using Logger = Core::Logging::LoggerManager;

int main(int argc, char *argv[]) {
  Logger::stream_set(Logger::stream_cout());
  Logger::level_set(Logger::Level::Info);
  auto sock = std::make_shared<Socket>();

  sock->on_listening += ([](auto server) {
    Logger::info("{}: Start Listening", server->name.c_str());
  });

  sock->on_accept += ([](auto serv, auto client) {
    Logger::info("{}: {}: Accepted", serv->name.c_str(), client->name.c_str());
    client->on_data += [](auto client, auto data) {
      Logger::info("{}: Data Received: {}", client->name.c_str(), data->size());
      client->write_buffer(*data);
      client->close();
    };
  });

  sock->listen(3000);

  return 0;
}

/**
 * @brief bash test cases
 * ( sleep 1; echo "first data"; sleep 1; echo "second data" ) | nc localhost
 * 3000
 *
 */