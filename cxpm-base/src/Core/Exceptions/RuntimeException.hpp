#pragma once

#include "Core/Containers/String.hpp"
#include "Utils/StringUtils/FormatString.hpp"
#include "Utils/Unused.hpp"
#include <exception>

namespace Core::Exceptions {
class RuntimeException : public std::exception {
public:
  template <typename... ArgsTypes>
  RuntimeException(const Utils::StringUtils::FormatString& fmt, ArgsTypes &&...args) : std::exception() {
    Utils::Unused{fmt, args...};
  }

  virtual const char *
  what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
    return _M_what.c_str();
  }

private:
  Containers::String _M_what;
};
} // namespace Core::Exceptions