#pragma once

#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Networking/HTTP/RequestDescriptor.hpp"
#include "Modules/Networking/HTTP/Response.hpp"
#include "Modules/Networking/HTTP/ResponseStatus.hpp"
#include "Utils/Patterns/Creator.hpp"
#include <functional>

using namespace Utils::Patterns;

namespace Modules::Networking::HTTP {
class Route : public Object,
              public EnableSharedFromThis<Route>,
              public Creator<Route> {
public:
  using CallbackType = std::function<void(Request &, Response &)>;

  Route() {}

  Route(const Method &method, const String &model, const CallbackType &callback,
        const Version &version = {1, 0}, const Map<String, String> &data = {})
      : method(method), version(version), model(model), callback(callback),
        compiled(compile(model)) {}

  Route(const Route &other)
      : method(other.method), version(other.version), model(other.model),
        callback(other.callback), compiled(other.compiled), data(other.data) {}

  Route &operator=(const Route &other) {
    method = other.method;
    version = other.version;
    model = other.model;
    callback = other.callback;
    compiled = other.compiled;
    data = other.data;
    return *this;
  }

  Route(const Route &&other)
      : method(other.method), version(other.version), model(other.model),
        callback(other.callback), compiled(other.compiled), data(other.data) {}

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
  Map<String, String> data;

  const Map<String, int> &context() { return compiled; }

  Map<String, String> parse(const String &resource) {

    auto result = Map<String, String>();
    auto haystack = String::split(resource, "/");

    if (context().size() == 0 && resource != model) {
      throw Exceptions::RuntimeException("Invalid Match {} : {}", model,
                                         resource);
    }

    if (context().size() > haystack.size() - 1) {
      throw Exceptions::RuntimeException("Invalid Match {} : {}", model,
                                         resource);
    }

    for (auto [key, value] : context()) {
      if (value < haystack.size()) {
        result[key] = haystack[value];
      }
    }

    if (result.size() != context().size()) {
      throw Exceptions::RuntimeException("Invalid Match {} : {}", model,
                                         resource);
    }

    return result;
  }

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