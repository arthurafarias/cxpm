#pragma once

#include <CXPM/Modules/Console/AbstractApplication.hpp>

namespace CXPM::Modules::Console {
class AbstractConsoleApplication : public AbstractApplication {
public:
  explicit AbstractConsoleApplication(int argc, char *argv[]) : AbstractApplication(argc, argv) {}
  virtual ~AbstractConsoleApplication() {}
};
} // namespace CXPM::Modules::Console