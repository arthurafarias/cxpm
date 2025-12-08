#pragma once

#include "Core/Exceptions/NotImplementedException.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Networking/HTTP/Method.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Networking/HTTP/Response.hpp"
#include "Utils/Patterns/Creator.hpp"
#include <functional>
#include <tuple>

using namespace Utils::Patterns;

namespace Modules::Networking::HTTP {
class Route : public Object,
              public EnableSharedFromThis<Route>,
              public Creator<Route> {
public:
  enum class RouteTagType { Valued, Star, Literal };

  using RouteTag = std::tuple<RouteTagType, int, String>;

  using CallbackType =
      std::function<void(SharedPointer<Request>, SharedPointer<Response>)>;

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

  const Map<String, RouteTag> &context() { return compiled; }

  Map<String, String> parse(const String &resource) {

    auto result = Map<String, String>();
    auto haystack = String::split(resource, "/");
    auto compiled = context();

    if (compiled.size() != haystack.size()) {
      throw Exceptions::RuntimeException("Couldn't match route {} to {}", model,
                                         resource);
    }

    for (auto [key, tag] : context()) {
      auto [type, index, value] = tag;

      switch (type) {
      case RouteTagType::Valued:
        result[value] = haystack[index];
        break;
      case RouteTagType::Literal:
        // result[value] = ""; // ignore literals but the size should be the
        // the validation is already really straight, if the number of tags
        // aren't the same number of tag in the compiled model it shouldn't be
        // interpreted as the route. That's enought and easier for now.
        if (value != haystack[index]) {
          throw Exceptions::RuntimeException("Couldn't parse {} from {}", resource, model);
        }
        break;
      case RouteTagType::Star:
        throw Exceptions::NotImplementedException(
            "Star Tag is not implemented yet.");
        break;
      }
    }

    return result;
  }

private:
  Map<String, RouteTag> compiled;

  inline static Map<String, RouteTag> compile(const String &model) {
    auto context = Map<String, RouteTag>();

    auto path_collection = String::split(model, "/");

    for (size_t i = 0; i < path_collection.size(); i++) {

      if (path_collection[i].starts_with(":")) {
        context[path_collection[i].substr(1, path_collection[i].size())] = {
            RouteTagType::Valued, i, path_collection[i]};
        continue;
      }

      /// TODO: Star Tag is not really easy to archieve I should join the rest
      /// and store into it like mqtt or any other index. Really complex. For
      /// now, something more easy.

      // if (path_collection[i].starts_with("*")) {
      //   context[path_collection[i].substr(1, path_collection[i].size())] = {
      //       RouteTagType::Star, i, path_collection[i]};
      //   continue;
      // }

      context[path_collection[i]] = {RouteTagType::Literal, i,
                                     path_collection[i]};
    }

    return context;
  }
};
} // namespace Modules::Networking::HTTP