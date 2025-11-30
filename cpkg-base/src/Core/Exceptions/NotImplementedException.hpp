#pragma once

#include "Core/Exceptions/RuntimeException.hpp"

namespace Core::Exceptions {
class NotImplementedException : public RuntimeException {
public:
  using RuntimeException::RuntimeException;
  NotImplementedException() : RuntimeException("Not Implemented!") {}
};
} // namespace Core::Exceptions