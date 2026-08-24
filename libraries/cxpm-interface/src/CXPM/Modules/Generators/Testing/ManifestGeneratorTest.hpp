#pragma once

#include "CXPM/Modules/Generators/ManifestGenerator.hpp"
#include "CXPM/Modules/Serialization/JsonManifest.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;
using namespace CXPM::Modules::Generators;
using namespace CXPM::Modules::Serialization;

namespace CXPM::Modules::Generators::Testing {

struct ManifestGeneratorTest : public TestGroup {
  ManifestGeneratorTest()
      : TestGroup(
            "ManifestGenerator",
            {
                {"generate_package_cpp_template embeds the target name and "
                 "uses the Target/Project builder API",
                 [](TestContext &ctx) {
                   auto text = generate_package_cpp_template("demo");
                   ctx.check(text.find("name_set(\"demo\")") != String::npos);
                   ctx.check(text.find("#include <CXPM/Target.hpp>") !=
                             String::npos);
                   ctx.check(text.find("Project().add(example).create()") !=
                             String::npos);
                 }},
                {"generate_package_json_template produces a manifest "
                 "ProjectManager::load_project() can parse",
                 [](TestContext &ctx) {
                   auto text = generate_package_json_template("demo");
                   auto project =
                       project_descriptor_from_json(parse_json(text));
                   ctx.equal(project.targets.size(), std::size_t{1});
                   ctx.equal(project.targets[0].name, String("demo"));
                   ctx.equal(project.targets[0].type, String("executable"));
                   ctx.equal(project.targets[0].sources.size(),
                             std::size_t{1});
                 }},
                {"generate_toolchain_cpp_template embeds the toolchain name",
                 [](TestContext &ctx) {
                   auto text = generate_toolchain_cpp_template("my-gcc");
                   ctx.check(text.find("name_set(\"my-gcc\")") !=
                             String::npos);
                   ctx.check(text.find("#include <CXPM/Toolchain.hpp>") !=
                             String::npos);
                 }},
                {"generate_toolchain_json_template produces a manifest "
                 "ToolchainManager can parse",
                 [](TestContext &ctx) {
                   auto text = generate_toolchain_json_template("my-gcc");
                   auto descriptor =
                       toolchain_descriptor_from_json(parse_json(text));
                   ctx.equal(descriptor.name, String("my-gcc"));
                   ctx.equal(descriptor.language, String("c++"));
                   ctx.check(!descriptor.compiler_executable.empty());
                 }},
            }) {}
};

inline static ManifestGeneratorTest manifest_generator_test_instance;

} // namespace CXPM::Modules::Generators::Testing
