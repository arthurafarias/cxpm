#pragma once

#include "CXPM/Modules/Serialization/JsonInputArchiver.hpp"
#include "CXPM/Modules/Serialization/JsonManifest.hpp"
#include "CXPM/Project.hpp"
#include "CXPM/Target.hpp"
#include "CXPM/Testing/TestGroup.hpp"
#include "CXPM/Toolchain.hpp"

#include <sstream>

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;
using namespace CXPM::Modules::Serialization;

namespace CXPM::Modules::Serialization::Testing {

struct JsonManifestTest : public TestGroup {
  JsonManifestTest()
      : TestGroup(
            "JsonManifest",
            {
                {"a TargetDescriptor round-trips through JSON",
                 [](TestContext &ctx) {
                   auto target = CXPM::Target()
                                     .name_set("example")
                                     .version_set("1.0.0")
                                     .type_set("executable")
                                     .sources_append({"src/main.cpp"})
                                     .link_libraries_append("m")
                                     .dependencies_append("gstreamer-1.0")
                                     .create();

                   auto json = write_json(to_json(target));
                   auto roundtripped = target_descriptor_from_json(parse_json(json));

                   ctx.equal(roundtripped.name, String("example"));
                   ctx.equal(roundtripped.version, String("1.0.0"));
                   ctx.equal(roundtripped.type, String("executable"));
                   ctx.equal(roundtripped.sources.size(), std::size_t{1});
                   ctx.equal(roundtripped.sources[0], String("src/main.cpp"));
                   ctx.equal(roundtripped.link_libraries[0], String("m"));
                   ctx.equal(roundtripped.dependencies[0],
                             String("gstreamer-1.0"));
                 }},
                {"a TargetDescriptor parsed from a partial JSON object keeps "
                 "the documented defaults for missing fields",
                 [](TestContext &ctx) {
                   auto descriptor =
                       target_descriptor_from_json(parse_json(R"({"name":"x"})"));
                   ctx.equal(descriptor.name, String("x"));
                   ctx.equal(descriptor.type, String("executable"));
                   ctx.equal(descriptor.language, String("c++"));
                   ctx.equal(descriptor.toolchain, String("g++"));
                 }},
                {"a ToolchainDescriptor round-trips through JSON",
                 [](TestContext &ctx) {
                   auto toolchain = CXPM::Toolchain()
                                        .name_set("g++")
                                        .version_set("generic")
                                        .compiler_executable_set("/usr/bin/g++")
                                        .linker_executable_set("/usr/bin/g++")
                                        .archiver_executable_set("/usr/bin/ar");

                   auto json = write_json(to_json(
                       static_cast<const CXPM::ToolchainDescriptor &>(
                           toolchain)));
                   auto roundtripped =
                       toolchain_descriptor_from_json(parse_json(json));

                   ctx.equal(roundtripped.name, String("g++"));
                   ctx.equal(roundtripped.compiler_executable,
                             String("/usr/bin/g++"));
                   ctx.equal(roundtripped.archiver_executable,
                             String("/usr/bin/ar"));
                   ctx.equal(roundtripped.shared_object_prefix, String("lib"));
                   ctx.equal(roundtripped.object_suffix, String(".o"));
                 }},
                {"a ProjectDescriptor with multiple targets and toolchains "
                 "round-trips through JSON",
                 [](TestContext &ctx) {
                   auto first =
                       CXPM::Target().name_set("first").create();
                   auto second =
                       CXPM::Target().name_set("second").create();
                   auto toolchain =
                       CXPM::Toolchain().name_set("g++");

                   auto project = CXPM::Project()
                                      .add(first)
                                      .add(second)
                                      .add(static_cast<const CXPM::ToolchainDescriptor &>(
                                          toolchain))
                                      .create();

                   auto json = write_json(to_json(
                       static_cast<const CXPM::ProjectDescriptor &>(project)));
                   auto roundtripped = project_descriptor_from_json(parse_json(json));

                   ctx.equal(roundtripped.targets.size(), std::size_t{2});
                   ctx.equal(roundtripped.targets[0].name, String("first"));
                   ctx.equal(roundtripped.targets[1].name, String("second"));
                   ctx.equal(roundtripped.toolchains.size(), std::size_t{1});
                   ctx.equal(roundtripped.toolchains[0].name, String("g++"));
                 }},
                {"a malformed field type raises JsonParseException",
                 [](TestContext &ctx) {
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>([] {
                     target_descriptor_from_json(
                         parse_json(R"({"name": 42})"));
                   });
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>([] {
                     target_descriptor_from_json(parse_json(R"("not an object")"));
                   });
                 }},
                {"JsonInputArchiver reads an istream into a Value tree",
                 [](TestContext &ctx) {
                   std::istringstream stream(R"({"name":"example"})");
                   JsonInputArchiver archiver(stream);
                   const auto &object =
                       std::get<Map<String, Value>>(archiver.value());
                   ctx.equal(std::get<String>(object.at("name")),
                             String("example"));
                 }},
            }) {}
};

inline static JsonManifestTest json_manifest_test_instance;

} // namespace CXPM::Modules::Serialization::Testing
