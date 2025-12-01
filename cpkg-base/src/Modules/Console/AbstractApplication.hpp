#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Object.hpp"

using namespace Core::Containers;

namespace Modules::Console {
class AbstractApplication : public Object {

public:
  explicit AbstractApplication(int argc, char *argv[])
      : Object(), args_property(argv, argv + argc) {}
  virtual ~AbstractApplication() {}

  virtual int run() = 0;

  virtual const Collection<String> &args() { return args_property; }

protected:
  virtual int setup() = 0;

  Collection<String> args_property;
};
} // namespace Modules::Console