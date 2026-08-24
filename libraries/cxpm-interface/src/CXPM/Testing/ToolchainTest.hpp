#pragma once

// Toolchain's object_build/shared_object_link/archive_link honor `dry` by skipping
// ShellManager::exec entirely; executable_link passes `dry` through to ShellManager::exec, which
// itself short-circuits under dry (see ShellManagerTest.hpp) -- so every command-construction
// path here is exercisable with no real compiler invoked, no filesystem writes, and no process
// spawned. What genuinely needs a real compiler (actually invoking object_build/executable_link/
// shared_object_link/archive_link with dry=false) stays in the CLI integration tier.
//
// Toolchain::build(TargetDescriptor)'s pkg-config dependency-resolution loop is NOT gated by
// `dry` (a real inconsistency, matching the dry-handling inconsistency documented as
// SRS-architecture.md item A24) -- every target used below has an empty `dependencies` list so
// these tests never shell out to a real `pkg-config`.

#include "CXPM/Testing/TestGroup.hpp"
#include "CXPM/Toolchain.hpp"

using namespace CXPM::Core::Containers;

namespace CXPM::Testing {

namespace {

CXPM::Toolchain make_test_toolchain() {
  return CXPM::Toolchain()
      .compiler_executable_set("cc")
      .linker_executable_set("cc")
      .archiver_executable_set("ar");
}

CXPM::TargetDescriptor make_test_target(const String &type) {
  CXPM::TargetDescriptor target;
  target.name = "demo";
  target.project_path = "/tmp/cxpm-toolchain-test";
  target.sources = {"main.cpp"};
  target.include_directories = {"src"};
  target.link_directories = {"libdir"};
  target.link_libraries = {"m"};
  target.options = {"-Wall"};
  target.type = type;
  return target;
}

} // namespace

struct ToolchainTest : public TestGroup {
  ToolchainTest()
      : TestGroup(
            "Toolchain",
            {
                {"object_build(dry=true) constructs the full compiler command "
                 "line and reports success without shelling out",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("executable");

                   auto [status, command] =
                       toolchain.object_build("main.cpp", target, true);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.check(command.command.find("cc") != String::npos);
                   ctx.check(command.command.find("-Wall") != String::npos);
                   ctx.check(command.command.find("-I") != String::npos);
                   ctx.check(command.command.find("-c") != String::npos);
                   ctx.check(command.command.find("main.cpp") != String::npos);
                   ctx.equal(command.file, String("main.cpp"));
                   ctx.equal(command.output, String("main.cpp.o"));
                 }},
                {"executable_link(dry=true) constructs a link command naming "
                 "the target as its output",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("executable");

                   auto [status, command] =
                       toolchain.executable_link(target, true);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.check(command.command.find("-o") != String::npos);
                   ctx.check(command.command.find("demo") != String::npos);
                   ctx.equal(command.output, String("demo"));
                 }},
                {"shared_object_link(dry=true) names its output with the "
                 "lib<name>.so convention",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("shared-library");

                   auto [status, command] =
                       toolchain.shared_object_link(target, true);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.equal(command.output, String("libdemo.so"));
                 }},
                {"archive_link(dry=true) names its output with the <name>.a "
                 "convention",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("static-library");

                   auto [status, command] = toolchain.archive_link(target, true);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.check(command.command.find("ar") != String::npos);
                   ctx.equal(command.output, String("demo.a"));
                 }},
                {"build(TargetDescriptor, dry=true) dispatches to "
                 "executable_link for an executable target",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("executable");

                   auto [status, commands] = toolchain.build(target, true);

                   ctx.check(status == CXPM::Status::Success);
                   // Only the final link command is returned on a successful build --
                   // the per-source object_build commands are checked for success but not
                   // accumulated here (see the note above ToolchainTest's declaration).
                   ctx.equal(commands.size(), std::size_t{1});
                   ctx.equal(commands[0].output, String("demo"));
                 }},
                {"build(TargetDescriptor, dry=true) dispatches to "
                 "shared_object_link for a shared-library target",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("shared-library");

                   auto [status, commands] = toolchain.build(target, true);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.equal(commands.size(), std::size_t{1});
                   ctx.equal(commands[0].output, String("libdemo.so"));
                 }},
                {"build(TargetDescriptor, dry=true) dispatches to "
                 "archive_link for a static-library target",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("static-library");

                   auto [status, commands] = toolchain.build(target, true);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.equal(commands.size(), std::size_t{1});
                   ctx.equal(commands[0].output, String("demo.a"));
                 }},
                {"build(TargetDescriptor, dry=true) fails cleanly for an "
                 "unrecognized target type",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("not-a-real-target-type");

                   auto [status, commands] = toolchain.build(target, true);

                   ctx.check(status == CXPM::Status::Failure);
                   ctx.check(commands.empty());
                 }},
                {"build(ProjectDescriptor, dry=true) aggregates results "
                 "across every target",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();

                   CXPM::ProjectDescriptor project;
                   auto first = make_test_target("executable");
                   first.name = "first";
                   auto second = make_test_target("static-library");
                   second.name = "second";
                   project.targets = {first, second};

                   auto [status, commands] = toolchain.build(project, true);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.equal(commands.size(), std::size_t{2});
                 }},
                {"the _async variants wrap their synchronous counterparts "
                 "and resolve to the same result",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();
                   auto target = make_test_target("executable");

                   auto object_future =
                       toolchain.object_build_async("main.cpp", target, true);
                   auto [object_status, object_command] = object_future.get();
                   ctx.check(object_status == CXPM::Status::Success);
                   ctx.equal(object_command.output, String("main.cpp.o"));

                   // executable_link_async(), unlike its object/shared-object/archive
                   // siblings, takes no `dry` parameter at all -- it is structurally
                   // impossible to request an async dry-run executable link (see
                   // SRS-architecture.md item A25). Its always-real invocation is made
                   // hermetic here by using the POSIX `true` utility as the "compiler" --
                   // it ignores every argument and always exits 0, so no real compiler or
                   // filesystem state is required, unlike an actual g++/clang++ invocation.
                   auto true_toolchain =
                       CXPM::Toolchain().compiler_executable_set("true");
                   auto link_future =
                       true_toolchain.executable_link_async(target);
                   auto [link_status, link_command] = link_future.get();
                   ctx.check(link_status == CXPM::Status::Success);
                   ctx.equal(link_command.output, String("demo"));

                   auto shared_target = make_test_target("shared-library");
                   auto shared_future =
                       toolchain.shared_object_link_async(shared_target, true);
                   auto [shared_status, shared_command] = shared_future.get();
                   ctx.check(shared_status == CXPM::Status::Success);
                   ctx.equal(shared_command.output, String("libdemo.so"));

                   auto archive_target = make_test_target("static-library");
                   auto archive_future =
                       toolchain.archive_link_async(archive_target, true);
                   auto [archive_status, archive_command] = archive_future.get();
                   ctx.check(archive_status == CXPM::Status::Success);
                   ctx.equal(archive_command.output, String("demo.a"));
                 }},
                {"install() is unimplemented for both ProjectDescriptor and "
                 "TargetDescriptor (real install logic lives entirely in "
                 "ProjectManager::install_target instead, see "
                 "SRS-architecture.md item A10)",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();

                   ctx.throws<CXPM::Core::Exceptions::NotImplementedException>(
                       [&toolchain] {
                         CXPM::ProjectDescriptor project;
                         toolchain.install(project);
                       });
                   ctx.throws<CXPM::Core::Exceptions::NotImplementedException>(
                       [&toolchain] {
                         auto target = make_test_target("executable");
                         toolchain.install(target);
                       });
                 }},
                {"build(ProjectDescriptor, dry=true) stops at the first "
                 "target that fails",
                 [](TestContext &ctx) {
                   auto toolchain = make_test_toolchain();

                   CXPM::ProjectDescriptor project;
                   auto bad = make_test_target("not-a-real-target-type");
                   bad.name = "bad";
                   auto good = make_test_target("executable");
                   good.name = "good";
                   project.targets = {bad, good};

                   auto [status, commands] = toolchain.build(project, true);

                   ctx.check(status == CXPM::Status::Failure);
                 }},
            }) {}
};

inline static ToolchainTest toolchain_test_instance;

} // namespace CXPM::Testing
