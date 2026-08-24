#pragma once

#include "CXPM/Testing/Registry.hpp"
#include "CXPM/Testing/TestCase.hpp"

#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace CXPM::Testing {

struct TestGroup {
public:
  TestGroup(std::string name, std::initializer_list<TestCase> tests)
      : name_(std::move(name)), tests_(tests) {
    registry().push_back(this);
  }
  virtual ~TestGroup() = default;

  [[nodiscard]] std::string_view name() const noexcept { return name_; }
  [[nodiscard]] const std::vector<TestCase> &tests() const noexcept {
    return tests_;
  }

private:
  std::string name_;
  std::vector<TestCase> tests_;
};

} // namespace CXPM::Testing
