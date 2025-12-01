#pragma once

#include "Modules/Console/AbstractConsoleApplication.hpp"

using namespace Modules::Console;

namespace Views {
class ApplicationView : public AbstractConsoleApplication {
public:
  ApplicationView(int argc, char *argv[])
      : AbstractConsoleApplication(argc, argv) {}

  virtual int run() override {
    auto lk = acquire_lock();

    setup();

    return 0;
  }

protected:
  int setup() override { return 0; }
};
} // namespace Views