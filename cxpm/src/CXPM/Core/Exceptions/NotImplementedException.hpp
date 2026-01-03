#pragma once

#include <CXPM/Core/Exceptions/RuntimeException.hpp>

namespace CXPM::Core::Exceptions {
class NotImplementedException : public RuntimeException {
public:
  using RuntimeException::RuntimeException;
  NotImplementedException() : RuntimeException("Not Implemented!") {}
};
} // namespace CXPM::Core::Exceptions