
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Route.hpp"
#include "Modules/Networking/HTTP/Service.hpp"
#include "Modules/Serialization/JSON/JSONObject.hpp"
#include "Modules/Serialization/JSON/JSONOutputArchiver.hpp"
#include <Utils/Unused.hpp>
#include <initializer_list>
#include <sstream>
// #include <string>

using namespace Modules::Networking::HTTP;
using namespace Core::Containers;
using namespace Modules::Serialization::JSON;

struct JSONValue;

int main(int argc, char *argv[]) {

  Utils::Unused{argc, argv};

  auto route0 = Modules::Networking::HTTP::Route::create(
      Method::GET, "/", [&](const Request &, Response &) {});

  auto route1 = Modules::Networking::HTTP::Route::create(
      Method::GET, "/:user/:sessionid/profile",
      [&](Request &req, Response &res) {
        std::stringstream ss;

        JsonOutputArchiver output(ss);
        JSONObject profile;

        profile["user"] = req.data["user"];
        profile["sessionid"] = req.data["sessionid"];
        profile["age"] = 35;

        output % profile;

        res.descriptor.body = ss.str();
        res.send();
      });

  auto route2 = Modules::Networking::HTTP::Route::create(
      Method::GET, "/:user/:sessionid", [&](const Request &, Response &) {});

  auto [result, err] =
      Service::create(
          std::initializer_list<SharedPointer<Route>>{route0, route1, route2})
          ->add_listener(8080, "127.0.0.1")
          ->run();

  return 0;
}