#pragma once

#include "Core/Object.hpp"
#include "Core/SharedPointer.hpp"
#include "Utils/Patterns/Creator.hpp"

namespace Utils::Patterns {
template <typename Derived>
struct Prototype : public Utils::Patterns::Creator<Derived>,
                   public EnableSharedFromThis<Derived> {
  virtual SharedPointer<Derived> clone() {
    return EnableSharedFromThis<Derived>::shared_from_this();
  }
};
} // namespace Utils::Patterns