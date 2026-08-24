#pragma once

#include "CXPM/Core/Containers/String.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Core::Containers::Testing {

struct StringTest : public TestGroup {
  StringTest()
      : TestGroup(
            "String",
            {
                {"join concatenates with delimiter",
                 [](TestContext &ctx) {
                   BasicCollection<String> parts{"a", "b", "c"};
                   ctx.equal(String::join(parts, "-"), String("a-b-c"));
                 }},
                {"join of a single element returns that element",
                 [](TestContext &ctx) {
                   BasicCollection<String> parts{"only"};
                   ctx.equal(String::join(parts, ","), String("only"));
                 }},
                {"join of an empty collection returns an empty string "
                 "instead of invoking undefined behavior",
                 [](TestContext &ctx) {
                   BasicCollection<String> parts;
                   ctx.equal(String::join(parts, ","), String(""));
                 }},
                {"trim removes leading and trailing non-printable characters",
                 [](TestContext &ctx) {
                   ctx.equal(String::trim("  hello  "), String("hello"));
                 }},
                {"trim_left removes only leading whitespace",
                 [](TestContext &ctx) {
                   ctx.equal(String::trim_left("  hello  "), String("hello  "));
                 }},
                {"trim_right removes only trailing whitespace",
                 [](TestContext &ctx) {
                   ctx.equal(String::trim_right("  hello  "), String("  hello"));
                 }},
                {"trim_right on an all-whitespace string returns an empty "
                 "string without underflowing",
                 [](TestContext &ctx) {
                   ctx.equal(String::trim_right("   "), String(""));
                 }},
                {"trim_right on an empty string returns an empty string",
                 [](TestContext &ctx) {
                   ctx.equal(String::trim_right(""), String(""));
                 }},
                {"split breaks the haystack on every needle occurrence and "
                 "trims each element",
                 [](TestContext &ctx) {
                   auto parts = String::split("a, b,c ,d", ",");
                   ctx.equal(parts.size(), std::size_t{4});
                   ctx.equal(parts[0], String("a"));
                   ctx.equal(parts[1], String("b"));
                   ctx.equal(parts[2], String("c"));
                   ctx.equal(parts[3], String("d"));
                 }},
                {"split with no needle occurrences returns the whole "
                 "string as a single element",
                 [](TestContext &ctx) {
                   auto parts = String::split("single", ",");
                   ctx.equal(parts.size(), std::size_t{1});
                   ctx.equal(parts[0], String("single"));
                 }},
            }) {}
};

inline static StringTest string_test_instance;

} // namespace CXPM::Core::Containers::Testing
