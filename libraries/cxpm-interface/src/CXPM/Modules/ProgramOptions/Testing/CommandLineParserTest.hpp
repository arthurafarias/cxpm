#pragma once

#include "CXPM/Modules/ProgramOptions/CommandLineParser.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Modules::ProgramOptions::Testing {

namespace {
CommandRegistry sample_registry() {
  CommandRegistry registry;

  registry.push_back({"build", "Build a project", "<directory>",
                      OptionsDescriptorCollection("build", "")});

  OptionsDescriptorCollection install_options("install", "");
  install_options.push_back({"prefix", "p", "/usr/local", "Install prefix"});
  registry.push_back(
      {"install", "Install a project", "<directory>", install_options});

  OptionsDescriptorCollection generate_options("generate", "");
  generate_options.push_back({"force", "f", "", "Overwrite an existing file"});
  registry.push_back({"generate", "Generate a manifest", "<kind> [directory]",
                      generate_options});

  return registry;
}
} // namespace

struct CommandLineParserTest : public TestGroup {
  CommandLineParserTest()
      : TestGroup(
            "ProgramOptions::CommandLineParser",
            {
                {"an unrecognized first token raises an error naming it and "
                 "pointing at 'help'",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   ctx.throws<CXPM::Core::Exceptions::RuntimeException>([&] {
                     parser.parse({"frobnicate"});
                   });
                 }},
                {"a recognized command's positional tokens are captured",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   auto parsed = parser.parse({"build", "."});
                   ctx.equal(parsed.command, String("build"));
                   ctx.equal(parsed.positionals.size(), std::size_t{1});
                   ctx.equal(parsed.positionals.front(), String("."));
                 }},
                {"an omitted option with a schema default is filled in "
                 "automatically",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   auto parsed = parser.parse({"install", "."});
                   ctx.check(parsed.has_option("prefix"));
                   ctx.equal(parsed.option_value("prefix"), String("/usr/local"));
                 }},
                {"an explicitly supplied option value overrides the schema "
                 "default",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   auto parsed =
                       parser.parse({"install", ".", "--prefix", "/opt/x"});
                   ctx.equal(parsed.option_value("prefix"), String("/opt/x"));
                 }},
                {"a short option name resolves to its long counterpart",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   auto parsed = parser.parse({"install", ".", "-p", "/opt/x"});
                   ctx.check(parsed.has_option("prefix"));
                   ctx.equal(parsed.option_value("prefix"), String("/opt/x"));
                 }},
                {"a valueless option with no schema default is present but "
                 "empty",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   auto parsed =
                       parser.parse({"generate", "package-json", ".", "--force"});
                   ctx.check(parsed.has_option("force"));
                   ctx.check(parsed.option_values("force").empty());
                 }},
                {"an option not declared for the command raises an error "
                 "naming the command",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   ctx.throws<CXPM::Core::Exceptions::RuntimeException>([&] {
                     parser.parse({"build", ".", "--bogus"});
                   });
                 }},
                {"no tokens produces the reserved help pseudo-command",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   auto parsed = parser.parse({});
                   ctx.equal(parsed.command, String("help"));
                   ctx.check(parsed.positionals.empty());
                 }},
                {"-h, --help and the bare word help all select the help "
                 "pseudo-command as the first token",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   ctx.equal(parser.parse({"-h"}).command, String("help"));
                   ctx.equal(parser.parse({"--help"}).command, String("help"));
                   ctx.equal(parser.parse({"help"}).command, String("help"));
                 }},
                {"help followed by a command name captures that name as a "
                 "positional",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   auto parsed = parser.parse({"help", "build"});
                   ctx.equal(parsed.command, String("help"));
                   ctx.equal(parsed.positionals.front(), String("build"));
                 }},
                {"-h after a real command sets the reserved help option "
                 "without changing the command",
                 [](TestContext &ctx) {
                   CommandLineParser parser(sample_registry());
                   auto parsed = parser.parse({"build", "-h"});
                   ctx.equal(parsed.command, String("build"));
                   ctx.check(parsed.has_option("help"));
                 }},
            }) {}
};

inline static CommandLineParserTest command_line_parser_test_instance;

} // namespace CXPM::Modules::ProgramOptions::Testing
