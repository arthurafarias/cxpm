
#include "Modules/Networking/HTTP/Service.hpp"
#include <Utils/Unused.hpp>
// #include <string>

class APIService : public Modules::Networking::HTTP::Service {
public:
  APIService(const String &base_url) : Modules::Networking::HTTP::Service() {
    UNUSED(base_url);

    route_add(Modules::Networking::HTTP::Method::GET, "/list",
              [](const Modules::Networking::HTTP::Request &req,
                 Modules::Networking::HTTP::Response &res) {
                UNUSED(req);
                UNUSED(res);

                res.descriptor.status.code = 200;

                res.send();
              });

    route_add(Modules::Networking::HTTP::Method::GET, "/:list",
              [](const Modules::Networking::HTTP::Request &req,
                 Modules::Networking::HTTP::Response &res) {
                UNUSED(req);
                UNUSED(res);

                res.descriptor.status.code = 200;

                res.send();
              });
  }

  using Modules::Networking::HTTP::Service::Service;
};

int main(int argc, char *argv[]) {

  Utils::Unused{argc, argv};

  auto [result, err] = APIService().add_listener(8080, "127.0.0.1").run();

  return 0;
}