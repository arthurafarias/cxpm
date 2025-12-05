#pragma once

#include <format>
#include <stdexcept>
#include <utility>

namespace Core::Exceptions {
class RuntimeException : public std::runtime_error {
public:
  template <typename... ArgsTypes>
  RuntimeException(const std::format_string<ArgsTypes...> &fmt,
                  ArgsTypes &&...args)
      : std::runtime_error(
            std::format(fmt, std::forward<ArgsTypes>(args)...)) {}
};
} // namespace Core::Exceptions