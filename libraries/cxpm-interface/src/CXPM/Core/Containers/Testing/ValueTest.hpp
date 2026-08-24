#pragma once

#include "CXPM/Core/Containers/Value.hpp"
#include "CXPM/Testing/TestGroup.hpp"

#include <variant>

using namespace CXPM::Testing;

namespace CXPM::Core::Containers::Testing {

using CxpmValue = CXPM::Core::Containers::Value;
using CxpmString = CXPM::Core::Containers::String;

struct ValueTest : public TestGroup {
  ValueTest()
      : TestGroup(
            "Value",
            {
                {"a default-constructed Value holds its first alternative "
                 "(nullptr_t)",
                 [](TestContext &ctx) {
                   CxpmValue value;
                   ctx.check(std::holds_alternative<std::nullptr_t>(value));
                 }},
                {"a Value constructed from a bool holds a bool alternative",
                 [](TestContext &ctx) {
                   CxpmValue value(true);
                   ctx.check(std::holds_alternative<bool>(value));
                   ctx.equal(std::get<bool>(value), true);
                 }},
                {"a Value constructed from an int holds an int alternative",
                 [](TestContext &ctx) {
                   CxpmValue value(42);
                   ctx.check(std::holds_alternative<int>(value));
                   ctx.equal(std::get<int>(value), 42);
                 }},
                {"a Value constructed from a double holds a double "
                 "alternative",
                 [](TestContext &ctx) {
                   CxpmValue value(3.5);
                   ctx.check(std::holds_alternative<double>(value));
                   ctx.equal(std::get<double>(value), 3.5);
                 }},
                {"a Value constructed from a String holds a String "
                 "alternative",
                 [](TestContext &ctx) {
                   CxpmValue value(CxpmString("hello"));
                   ctx.check(std::holds_alternative<CxpmString>(value));
                   ctx.equal(std::get<CxpmString>(value),
                             CxpmString("hello"));
                 }},
                {"std::get on the wrong alternative throws "
                 "std::bad_variant_access",
                 [](TestContext &ctx) {
                   CxpmValue value(42);
                   ctx.throws<std::bad_variant_access>(
                       [&value] { std::get<CxpmString>(value); });
                 }},
            }) {}
};

inline static ValueTest value_test_instance;

} // namespace CXPM::Core::Containers::Testing
