#pragma once

#include "CXPM/Core/Functional/Function.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Testing;

namespace CXPM::Core::Functional::Testing {

struct FunctionTest : public TestGroup {
  FunctionTest()
      : TestGroup(
            "Function",
            {
                {"wraps and invokes a lambda like std::function",
                 [](TestContext &ctx) {
                   Function<int(int, int)> add = [](int a, int b) { return a + b; };
                   ctx.equal(add(2, 3), 5);
                 }},
                {"a default-constructed Function is empty, like "
                 "std::function",
                 [](TestContext &ctx) {
                   Function<void()> empty;
                   ctx.check(!static_cast<bool>(empty));
                 }},
                {"is reassignable after construction",
                 [](TestContext &ctx) {
                   Function<int()> value = [] { return 1; };
                   value = [] { return 2; };
                   ctx.equal(value(), 2);
                 }},
            }) {}
};

inline static FunctionTest function_test_instance;

} // namespace CXPM::Core::Functional::Testing
