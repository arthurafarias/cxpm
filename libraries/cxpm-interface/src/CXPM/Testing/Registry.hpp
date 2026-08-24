#pragma once

#include <vector>

namespace CXPM::Testing {

struct TestGroup;

inline std::vector<const TestGroup *> &registry() {
  static std::vector<const TestGroup *> groups;
  return groups;
}

} // namespace CXPM::Testing
