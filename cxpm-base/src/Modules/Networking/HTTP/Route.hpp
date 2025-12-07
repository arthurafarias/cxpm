#pragma once

#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Networking/HTTP/Response.hpp"
#include <functional>
namespace Modules::Networking::HTTP {
class Route {
public:
  using CallbackType = std::function<void(const Request &, Response &)>;

  Route() {}

  Route(const Method &method, const Version &version, const String &model,
        const CallbackType &callback)
      : method(method), version(version), model(model), callback(callback),
        compiled(compile(model)) {}

  Route(const Route &other)
      : method(other.method), version(other.version), model(other.model),
        callback(other.callback), compiled(other.compiled) {}

  Route &operator=(const Route &other) {
    method = other.method;
    version = other.version;
    model = other.model;
    callback = other.callback;
    compiled = other.compiled;
    return *this;
  }

  Route(const Route &&other)
      : method(other.method), version(other.version), model(other.model),
        callback(other.callback), compiled(other.compiled) {}

  Route &operator=(const Route &&other) {
    method = other.method;
    version = other.version;
    model = other.model;
    callback = other.callback;
    compiled = other.compiled;
    return *this;
  }

  Method method;
  Version version;
  String model;
  CallbackType callback;

  const Map<String, int> &context() { return compiled; }

private:
  Map<String, int> compiled;

  inline static Map<String, int> compile(const String &model) {
    auto context = Map<String, int>();

    auto path_collection = String::split(model, "/");

    for (size_t i = 0; i < path_collection.size(); i++) {
      if (path_collection[i].starts_with(":")) {
        context[path_collection[i].substr(1, path_collection[i].size())] = i;
      }
    }

    return context;
  }
};
} // namespace Modules::Networking::HTTP