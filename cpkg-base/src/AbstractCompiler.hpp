#pragma once

#include <Package.hpp>

class AbstractCompiler {
public:
  virtual int build(const Package &package) = 0;
};