#pragma once

#include "CXPM/Testing/TestContext.hpp"

#include <functional>
#include <string>

namespace CXPM::Testing {

struct TestCase {
  std::string name;
  std::function<void(TestContext &)> run;
};

} // namespace CXPM::Testing
