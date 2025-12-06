#pragma once

#include <map>
#include <string>

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

inline std::map<Method, std::string> MethodMap = {
    {Method::GET, "GET"},       {Method::POST, "POST"},
    {Method::PUT, "POST"},      {Method::DELETE, "DELETE"},
    {Method::UPDATE, "UPDATE"}, {Method::CONNECT, "CONNECT"},
    {Method::HEAD, "HEAD"},     {Method::OPTIONS, "OPTIONS"},
    {Method::TRACE, "TRACE"},
};
} // namespace Modules::Networking::HTTP

namespace std {
inline std::string to_string(const Modules::Networking::HTTP::Method &method) {
  return Modules::Networking::HTTP::MethodMap[method];
}
} // namespace std