#pragma once

#include "Modules/Console/AbstractApplication.hpp"

namespace Modules::Console {
class AbstractConsoleApplication : public AbstractApplication {
public:
  explicit AbstractConsoleApplication(int argc, char *argv[]) : AbstractApplication(argc, argv) {}
  virtual ~AbstractConsoleApplication() {}
};
} // namespace Modules::Console