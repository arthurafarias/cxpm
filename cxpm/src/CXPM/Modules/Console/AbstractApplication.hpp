#pragma once

#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Core/Object.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::Console {
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
} // namespace CXPM::Modules::Console