#pragma once

#include "CXPM/Testing/TestFailure.hpp"

#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace CXPM::Testing {

struct TestContext {
public:
  void check(bool condition, std::string_view message = "check failed") const {
    if (!condition) {
      throw TestFailure(std::string{message});
    }
  }

  template <typename LeftType, typename RightType>
  void equal(const LeftType &left, const RightType &right,
             std::string_view message = "values differ") const {
    if (!(left == right)) {
      std::ostringstream stream;
      stream << message;
      throw TestFailure(stream.str());
    }
  }

  template <typename ExceptionType, typename FunctionType>
  void throws(FunctionType &&function,
              std::string_view message =
                  "expected exception was not thrown") const {
    try {
      std::forward<FunctionType>(function)();
    } catch (const ExceptionType &) {
      return;
    }
    throw TestFailure(std::string{message});
  }
};

} // namespace CXPM::Testing
