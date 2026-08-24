#pragma once

#include "CXPM/CompilerCommandDescriptor.hpp"
#include "CXPM/Modules/Serialization/JsonOutputArchiver.hpp"
#include "CXPM/Testing/TestGroup.hpp"

#include <sstream>

using namespace CXPM::Core::Containers;

namespace CXPM::Testing {

struct CompilerCommandDescriptorTest : public TestGroup {
  CompilerCommandDescriptorTest()
      : TestGroup(
            "CompileCommandDescriptor",
            {
                {"operator% serializes every field as a JSON object",
                 [](TestContext &ctx) {
                   const CXPM::CompileCommandDescriptor descriptor{
                       .directory = "/tmp/build",
                       .command = "g++ -c main.cpp",
                       .file = "main.cpp",
                       .output = "main.o",
                       .stdout = "",
                       .stderr = "",
                   };

                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   archiver %
                       CXPM::Modules::Serialization::ValueToken{descriptor};

                   auto text = stream.str();
                   ctx.check(text.starts_with("{"));
                   ctx.check(text.ends_with("}"));
                   ctx.check(text.find("\"directory\":\"/tmp/build\"") !=
                                  std::string::npos,
                              "missing directory field");
                   ctx.check(text.find("\"command\":\"g++ -c main.cpp\"") !=
                                  std::string::npos,
                              "missing command field");
                   ctx.check(text.find("\"file\":\"main.cpp\"") !=
                                  std::string::npos,
                              "missing file field");
                   ctx.check(text.find("\"output\":\"main.o\"") !=
                                  std::string::npos,
                              "missing output field");
                 }},
            }) {}
};

inline static CompilerCommandDescriptorTest
    compiler_command_descriptor_test_instance;

} // namespace CXPM::Testing
