#pragma once

#include "CXPM/Modules/ProgramOptions/Parse.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Modules::ProgramOptions::Testing {

struct ParseTest : public TestGroup {
  ParseTest()
      : TestGroup(
            "ProgramOptions::Parse",
            {
                {"a long flag followed by a value is captured under its name",
                 [](TestContext &ctx) {
                   auto options = CXPM::Modules::ProgramOptions::Parse(
                       {"--build", "."});
                   ctx.check(options.contains("build"));
                   ctx.equal(options["build"].size(), std::size_t{1});
                   ctx.equal(options["build"][0], String("."));
                 }},
                {"a short flag followed by a value is captured under its name",
                 [](TestContext &ctx) {
                   auto options =
                       CXPM::Modules::ProgramOptions::Parse({"-b", "."});
                   ctx.check(options.contains("b"));
                   ctx.equal(options["b"][0], String("."));
                 }},
                {"tokens before any flag are collected as positional "
                 "arguments",
                 [](TestContext &ctx) {
                   auto options =
                       CXPM::Modules::ProgramOptions::Parse({"foo", "bar"});
                   ctx.check(options.contains("__positional__"));
                   ctx.equal(options["__positional__"].size(), std::size_t{2});
                 }},
                {"a valueless flag that is the last token is still present "
                 "in the result",
                 [](TestContext &ctx) {
                   auto options =
                       CXPM::Modules::ProgramOptions::Parse({"--help"});
                   ctx.check(options.contains("help"));
                   ctx.check(options["help"].empty());
                 }},
                {"a valueless flag immediately followed by another flag is "
                 "still present in the result",
                 [](TestContext &ctx) {
                   auto options = CXPM::Modules::ProgramOptions::Parse(
                       {"--help", "--build", "."});
                   ctx.check(options.contains("help"));
                   ctx.check(options["help"].empty());
                   ctx.check(options.contains("build"));
                   ctx.equal(options["build"][0], String("."));
                 }},
            }) {}
};

inline static ParseTest parse_test_instance;

} // namespace CXPM::Modules::ProgramOptions::Testing
