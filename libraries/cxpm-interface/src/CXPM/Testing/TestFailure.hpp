#pragma once

#include <stdexcept>

namespace CXPM::Testing {

class TestFailure : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

} // namespace CXPM::Testing
