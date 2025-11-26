#pragma once

#include <Models/BasicTarget.hpp>

namespace Controllers {
using namespace Models;
class AbstractCompiler {
public:
  virtual int build(const BasicTarget &package) = 0;
};
} // namespace Controllers