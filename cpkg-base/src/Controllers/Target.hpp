#pragma once

#include <Models/BasicTarget.hpp>

namespace Controllers {
  using namespace Models;
class Target : public BasicTarget {
public:
  using BasicTarget::BasicTarget;
};
} // namespace Controllers

// extern Package package;