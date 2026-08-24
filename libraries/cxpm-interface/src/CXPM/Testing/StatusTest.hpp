#pragma once

#include "CXPM/Status.hpp"
#include "CXPM/Testing/TestGroup.hpp"

#include <string>

namespace CXPM::Testing {

struct StatusTest : public TestGroup {
  StatusTest()
      : TestGroup(
            "Status",
            {
                {"Success formats as Status::Success",
                 [](TestContext &ctx) {
                   ctx.equal(std::to_string(CXPM::Status::Success),
                             std::string("Status::Success"));
                 }},
                {"Failure formats as Status::Failure",
                 [](TestContext &ctx) {
                   ctx.equal(std::to_string(CXPM::Status::Failure),
                             std::string("Status::Failure"));
                 }},
            }) {}
};

inline static StatusTest status_test_instance;

} // namespace CXPM::Testing
