#pragma once

#include "CXPM/Modules/ProgramOptions/HelpFormatter.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Modules::ProgramOptions::Testing {

struct HelpFormatterTest : public TestGroup {
  HelpFormatterTest()
      : TestGroup(
            "ProgramOptions::HelpFormatter",
            {
                {"top_level names the program and lists every registered "
                 "command",
                 [](TestContext &ctx) {
                   CommandRegistry registry;
                   registry.push_back({"build", "Build a project", "<directory>",
                                       OptionsDescriptorCollection("build", "")});
                   registry.push_back({"help", "Show help", "[<command>]",
                                       OptionsDescriptorCollection("help", "")});

                   auto text = HelpFormatter::top_level("cxpm", "a description", registry);

                   ctx.check(text.find("cxpm: a description") != String::npos);
                   ctx.check(text.find("cxpm <command> [<args>]") != String::npos);
                   ctx.check(text.find("build") != String::npos);
                   ctx.check(text.find("help") != String::npos);
                 }},
                {"command's usage line includes the declared positional "
                 "placeholder",
                 [](TestContext &ctx) {
                   CommandDescriptor descriptor{"build", "Build a project",
                                                "<directory>",
                                                OptionsDescriptorCollection("build", "")};

                   auto text = HelpFormatter::command("cxpm", descriptor);

                   ctx.check(text.find("cxpm build <directory>") != String::npos);
                 }},
                {"command lists each declared option's long name and default",
                 [](TestContext &ctx) {
                   OptionsDescriptorCollection options("install", "");
                   options.push_back({"prefix", "p", "/usr/local", "Install prefix"});
                   CommandDescriptor descriptor{"install", "Install a project",
                                                "<directory>", options};

                   auto text = HelpFormatter::command("cxpm", descriptor);

                   ctx.check(text.find("-p, --prefix") != String::npos);
                   ctx.check(text.find("/usr/local") != String::npos);
                   ctx.check(text.find("Install prefix") != String::npos);
                 }},
                {"command always documents the reserved -h/--help flag even "
                 "when the schema declares no options",
                 [](TestContext &ctx) {
                   CommandDescriptor descriptor{"build", "Build a project",
                                                "<directory>",
                                                OptionsDescriptorCollection("build", "")};

                   auto text = HelpFormatter::command("cxpm", descriptor);

                   ctx.check(text.find("-h, --help") != String::npos);
                 }},
            }) {}
};

inline static HelpFormatterTest help_formatter_test_instance;

} // namespace CXPM::Modules::ProgramOptions::Testing
