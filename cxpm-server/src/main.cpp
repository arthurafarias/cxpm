
#include "Core/Containers/String.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/Globals.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Response.hpp"
#include "Modules/Networking/HTTP/ResponseStatus.hpp"
#include "Modules/Networking/HTTP/Route.hpp"
#include "Modules/Networking/HTTP/Service.hpp"
#include "Modules/Networking/TCP/Socket.hpp"
#include "Modules/SQL/Base/QueryBuilder.hpp"
#include "Modules/SQL/SQLite/SQLiteDriver.hpp"
#include "Modules/Serialization/JSON/JSONObject.hpp"
#include "Modules/Serialization/JSON/JSONOutputArchiver.hpp"
#include <Utils/Unused.hpp>
#include <csignal>
#include <ios>
#include <sstream>

using namespace Modules::Networking::HTTP;
using namespace Core::Containers;
using namespace Modules::Serialization::JSON;

struct JSONValue;

struct Router {

  SharedPointer<Modules::SQL::SQLite::SQLiteDriver> db;

  Router(const String &filename = "router.db") {
    using namespace Modules::SQL::SQLite;
    db = Modules::SQL::SQLite::SQLiteDriver::create(filename);

    {
      auto query = QueryBuilder::create()->create_table(
          R"(IF NOT EXISTS users
                          (
                            id INTEGER NOT NULL,
                            user STRING NOT NULL,
                            password STRING NOT NULL,
                            sessionid STRING NOT NULL
                          );)");
      db->query(query);
    }

    {
      auto query = QueryBuilder::create()
                       ->insert_into("users")
                       ->fields_start()
                       ->field("id")
                       ->field("user")
                       ->field("password")
                       ->field("sessionid")
                       ->fields_end()
                       ->values_start()
                       ->value("0")
                       ->value("root")
                       ->value("root")
                       ->value("")
                       ->values_end();

      Logging::LoggerManager::info("Executing Query: {}",
                                   query->compile().c_str());
      db->query(query);
    }

    {
      auto query = QueryBuilder::create()->create_table(
          R"(IF NOT EXISTS sessions
                          (
                            id INTEGER NOT NULL,
                            date STRING NOT NULL,
                            userid STRING NOT NULL
                          );)");
      db->query(query);
    }
  }

  void default_handler(SharedPointer<Socket> client, SharedPointer<Request> req,
                       SharedPointer<Response> res) {
    res->status = {200, "OK"};
    client->write_string(res->to_string());
  }

  void user_profile_get(SharedPointer<Socket> client,
                        SharedPointer<Request> req,
                        SharedPointer<Response> res) {
    auto lock0 = req->acquire_lock();
    auto lock1 = res->acquire_lock();
    std::stringstream ss;

    JSONOutputArchiver output(ss);
    JSONObject profile;

    QueryBuilder::create()
        ->from("user")
        ->select("*")
        ->order_by("sessionid")
        ->limit("1")
        ->where("sessionid = '{}'", req->data["sessionid"])
        ->exec(db);

    output << profile;

    res->body = ss.str();
    client->write_string(res->to_string());
  }

  const inline static Collection<String> EscapeSequences{
      "select", "insert", "delete", "order_by", "if", "else", "fi",
      ";",      "drop",   "@",      "'",        "`",  "\\",

  };

  inline static bool can_leak(const String &string) {
    auto lock = EscapeSequences.acquire_lock();

    std::ostringstream os;
    os << std::uppercase << string;

    if (find_if(EscapeSequences.begin(), EscapeSequences.end(),
                [](auto &el) { return false; }) != EscapeSequences.end())
      return true;

    return false;
  }

  void user_keep_alive(SharedPointer<Socket> client, SharedPointer<Request> req,
                       SharedPointer<Response> res) {
    auto lock0 = req->acquire_lock();
    auto lock1 = res->acquire_lock();
    client->write_string(res->to_string());
  }

  void user_log_out(SharedPointer<Socket> client, SharedPointer<Request> req,
                    SharedPointer<Response> res) {
    auto lock0 = req->acquire_lock();
    auto lock1 = res->acquire_lock();
    client->write_string(res->to_string());
  }

  void user_log_in(SharedPointer<Socket> client, SharedPointer<Request> req,
                   SharedPointer<Response> res) {
    auto lock0 = req->acquire_lock();
    auto lock1 = res->acquire_lock();
    std::stringstream ss;

    JSONOutputArchiver output(ss);
    JSONObject body;

    try {

      auto user = req->data["user"];
      auto password = req->data["password"];

      if (can_leak(user)) {
        throw Exceptions::RuntimeException("Username would inject sql {}.",
                                           user);
      }

      if (can_leak(password)) {
        throw Exceptions::RuntimeException("Password would inject sql {}.",
                                           password);
      }

      QueryBuilder::create()
          ->from("user")
          ->select("*")
          ->order_by("sessionid")
          ->limit("1")
          ->where("user = '{}' and password = '{}'", user, password)
          ->exec(db);

    } catch (std::exception &ex) {
      ss.clear();
      ss << String(std::format("Authentication Failed: {}", ex.what()));
    }

    res->body = ss.str();
    client->write_string(res->to_string());
  }
};

int main(int argc, char *argv[]) {
  Utils::Unused{argc, argv};
  ::signal(SIGTERM, Core::Globals::close);

  Logging::LoggerManager::level_set(Logging::LoggerManager::Level::Debug);
  // Logging::LoggerManager::stream_set(Logging::LoggerManager::stream_cout());

  auto router = SharedPointer<Router>(new Router());

  auto [result, err] =
      Service::create()
          ->listener_add(3000, "127.0.0.1")
          ->route_add(Method::GET, "/",
                      std::bind(&Router::default_handler, router,
                                std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3))
          ->route_add(Method::POST, "/login",
                      std::bind(&Router::user_log_in, router,
                                std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3))
          ->route_add(Method::GET, "/:user/:sessionid/profile",
                      std::bind(&Router::user_profile_get, router,
                                std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3))
          ->route_add(Method::POST, "/:user/:sessionid/keepalive",
                      std::bind(&Router::user_log_in, router,
                                std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3))
          ->route_add(Method::POST, "/:user/:sessionid/logout",
                      std::bind(&Router::user_log_in, router,
                                std::placeholders::_1, std::placeholders::_2,
                                std::placeholders::_3))
          ->run();

  return 0;
}