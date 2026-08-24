#pragma once

#include "CXPM/Core/Exceptions/RuntimeException.hpp"

namespace CXPM::Core::Exceptions {
class JsonParseException : public RuntimeException {
public:
  using RuntimeException::RuntimeException;
};
} // namespace CXPM::Core::Exceptions
