#pragma once

#include "CXPM/Project.hpp"
#include "CXPM/Target.hpp"
#include "CXPM/Testing/TestGroup.hpp"
#include "CXPM/Toolchain.hpp"

using namespace CXPM::Core::Containers;

namespace CXPM::Testing {

struct ProjectTest : public TestGroup {
  ProjectTest()
      : TestGroup(
            "Project",
            {
                {"add(TargetDescriptor) appends a target",
                 [](TestContext &ctx) {
                   auto target = CXPM::Target().name_set("example").create();
                   auto project = CXPM::Project().add(target).create();
                   ctx.equal(project.targets.size(), std::size_t{1});
                   ctx.equal(project.targets[0].name, String("example"));
                 }},
                {"add(ToolchainDescriptor) appends a toolchain",
                 [](TestContext &ctx) {
                   CXPM::Toolchain toolchain;
                   toolchain.name_set("g++");
                   auto project = CXPM::Project().add(toolchain).create();
                   ctx.equal(project.toolchains.size(), std::size_t{1});
                   ctx.equal(project.toolchains[0].name, String("g++"));
                 }},
                {"multiple targets and toolchains can be composed fluently",
                 [](TestContext &ctx) {
                   auto first = CXPM::Target().name_set("first").create();
                   auto second = CXPM::Target().name_set("second").create();
                   auto project =
                       CXPM::Project().add(first).add(second).create();
                   ctx.equal(project.targets.size(), std::size_t{2});
                 }},
                {"create() returns the same instance rather than a copy",
                 [](TestContext &ctx) {
                   CXPM::Project project;
                   auto &created = project.create();
                   ctx.check(&created == &project);
                 }},
            }) {}
};

inline static ProjectTest project_test_instance;

} // namespace CXPM::Testing
