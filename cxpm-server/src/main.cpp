
#include "Modules/Networking/HTTP/Service.hpp"
#include "Modules/Networking/TCP/Listener.hpp"
#include <Utils/Unused.hpp>
#include <string>

class APIService : public Modules::Networking::HTTP::Service {
public:
  APIService(const String &base_url) : Modules::Networking::HTTP::Service() {
    UNUSED(base_url);

    route_add(Modules::Networking::HTTP::Method::GET, "/list",
              [](const Modules::Networking::HTTP::Request &req,
                 Modules::Networking::HTTP::Response &res) {
                UNUSED(req);
                UNUSED(res);

                res.send(200);
              });
  }

  using Modules::Networking::HTTP::Service::Service;
};

int main(int argc, char *argv[]) {

  Utils::Unused{argc, argv};

  APIService http_service("http://localhost:8080");

  auto [result, err] = Modules::Networking::TCP::Listener()
                           .service_add(http_service)
                           .listen(8080, "localhost");

  if (result !=
      Modules::Networking::TCP::Listener::ListenResultStatus::Success) {
    return -1;
  }

  return 0;
}