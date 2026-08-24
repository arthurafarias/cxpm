#pragma once

#include "CXPM/Target.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;

namespace CXPM::Testing {

struct TargetTest : public TestGroup {
  TargetTest()
      : TestGroup(
            "Target",
            {
                {"fluent setters chain and are readable back through their "
                 "matching getters",
                 [](TestContext &ctx) {
                   auto target = CXPM::Target()
                                     .name_set("example")
                                     .version_set("1.0.0")
                                     .type_set("executable")
                                     .language_set("c++")
                                     .toolchain_set("g++")
                                     .install_prefix_set("/usr/local")
                                     .sources_append({"src/main.cpp"})
                                     .options_append({"-Wall", "-Wextra"})
                                     .include_directories_append("src")
                                     .link_directories_append("lib")
                                     .link_libraries_append("m")
                                     .create();

                   ctx.equal(target.name_get(), String("example"));
                   ctx.equal(target.version_get(), String("1.0.0"));
                   ctx.equal(target.type_get(), String("executable"));
                   ctx.equal(target.language_get(), String("c++"));
                   ctx.equal(target.toolchain_get(), String("g++"));
                   ctx.equal(target.install_prefix_get(),
                             String("/usr/local"));
                   ctx.equal(target.sources_get().size(), std::size_t{1});
                   ctx.equal(target.sources_get()[0], String("src/main.cpp"));
                   ctx.equal(target.options_get().size(), std::size_t{2});
                   ctx.equal(target.options_get()[0], String("-Wall"));
                   ctx.equal(target.options_get()[1], String("-Wextra"));
                   ctx.equal(target.include_directories_get()[0],
                             String("src"));
                   ctx.equal(target.link_directories_get()[0], String("lib"));
                   ctx.equal(target.link_libraries_get()[0], String("m"));
                 }},
                {"create() returns the same instance rather than a copy",
                 [](TestContext &ctx) {
                   CXPM::Target target;
                   auto &created = target.create();
                   ctx.check(&created == &target);
                 }},
                {"sources_append accumulates across multiple calls",
                 [](TestContext &ctx) {
                   auto target = CXPM::Target()
                                     .sources_append({"a.cpp"})
                                     .sources_append({"b.cpp", "c.cpp"});
                   ctx.equal(target.sources_get().size(), std::size_t{3});
                 }},
                {"defaults match the documented TargetDescriptor defaults",
                 [](TestContext &ctx) {
                   CXPM::Target target;
                   ctx.equal(target.type_get(), String("executable"));
                   ctx.equal(target.language_get(), String("c++"));
                   ctx.equal(target.toolchain_get(), String("g++"));
                 }},
                {"dependencies_set replaces the dependency list",
                 [](TestContext &ctx) {
                   auto target = CXPM::Target().dependencies_set(
                       {"gstreamer-1.0"});
                   ctx.equal(target.dependencies_get().size(),
                             std::size_t{1});
                   ctx.equal(target.dependencies_get()[0],
                             String("gstreamer-1.0"));
                 }},
                {"dependencies_append accumulates across multiple calls "
                 "(regression: this method was missing, see the "
                 "architecture SRS item A31)",
                 [](TestContext &ctx) {
                   auto target = CXPM::Target()
                                     .dependencies_append("gstreamer-1.0")
                                     .dependencies_append("glib-2.0");
                   ctx.equal(target.dependencies_get().size(),
                             std::size_t{2});
                   ctx.equal(target.dependencies_get()[0],
                             String("gstreamer-1.0"));
                   ctx.equal(target.dependencies_get()[1],
                             String("glib-2.0"));
                 }},
            }) {}
};

inline static TargetTest target_test_instance;

} // namespace CXPM::Testing
