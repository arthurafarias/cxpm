
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Route.hpp"
#include "Modules/Networking/HTTP/Service.hpp"
#include "Modules/Serialization/JSON/JSONObject.hpp"
#include "Modules/Serialization/JSON/JSONOutputArchiver.hpp"
#include <Utils/Unused.hpp>
#include <sstream>
// #include <string>

using namespace Modules::Networking::HTTP;
using namespace Core::Containers;
using namespace Modules::Serialization::JSON;

struct JSONValue;

struct Router {
  void user_profile_get(SharedPointer<Request> req,
                        SharedPointer<Response> res) {
    std::stringstream ss;

    JsonOutputArchiver output(ss);
    JSONObject profile;

    profile["user"] = req->data[":user"];
    profile["sessionid"] = req->data[":sessionid"];
    profile["age"] = 35;

    output % profile;

    res->body = ss.str();
    res->send();
  }

  void user_age_get(SharedPointer<Request> req,
                        SharedPointer<Response> res) {
    std::stringstream ss;

    JsonOutputArchiver output(ss);
    JSONObject profile;
    
    profile["age"] = 35;

    output % profile;

    res->body = ss.str();
    res->send();
  }
};

int main(int argc, char *argv[]) {

  Utils::Unused{argc, argv};

  auto router = SharedPointer<Router>(new Router());

  auto [result, err] =
      Service::create()
          ->listener_add(8080, "127.0.0.1")
          ->route_add(Method::GET, "/", [&](auto, auto) {})
          ->route_add(Method::GET, "/:user/:sessionid/profile",
                      std::bind(&Router::user_profile_get, router,
                                std::placeholders::_1, std::placeholders::_2))
          ->route_add(Method::GET, "/:user/:sessionid/age",
                      std::bind(&Router::user_age_get, router,
                                std::placeholders::_1, std::placeholders::_2))

          ->run();

  return 0;
}