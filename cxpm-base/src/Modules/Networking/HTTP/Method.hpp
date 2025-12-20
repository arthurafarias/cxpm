#pragma once

#include "Core/Containers/Map.hpp"
#include <string>

#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Modules::Networking::HTTP {

enum class Method {
  GET,
  POST,
  PUT,
  DELETE,
  UPDATE,
  CONNECT,
  HEAD,
  OPTIONS,
  TRACE
};

inline const Map<Method, String> MethodMap = {
    {Method::GET, "GET"},       {Method::POST, "POST"},
    {Method::PUT, "POST"},      {Method::DELETE, "DELETE"},
    {Method::UPDATE, "UPDATE"}, {Method::CONNECT, "CONNECT"},
    {Method::HEAD, "HEAD"},     {Method::OPTIONS, "OPTIONS"},
    {Method::TRACE, "TRACE"},
};

inline const Map<String, Method> MethodReverseMap = {
    {"GET", Method::GET},       {"POST", Method::POST},
    {"POST", Method::PUT},      {"DELETE", Method::DELETE},
    {"UPDATE", Method::UPDATE}, {"CONNECT", Method::CONNECT},
    {"HEAD", Method::HEAD},     {"OPTIONS", Method::OPTIONS},
    {"TRACE", Method::TRACE},
};

} // namespace Modules::Networking::HTTP

namespace std {
inline String to_string(const Modules::Networking::HTTP::Method &method) {
  return Modules::Networking::HTTP::MethodMap.at(method);
}
} // namespace std