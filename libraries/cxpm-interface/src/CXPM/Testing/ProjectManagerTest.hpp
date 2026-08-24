#pragma once

// install_target() only *copies* already-built artifacts -- it never invokes a compiler -- so it
// can be exercised hermetically by pre-creating the "already built" files a real `cxpm --build`
// would have produced, in a temp directory standing in for the project directory. This is also a
// regression test for a real bug found while writing it: install_target() used to nest its
// artifact/.pc install logic *inside* the per-include-directory loop, so a target with an empty
// include_directories list silently installed nothing at all (see docs/SRS-architecture.md).

#include "CXPM/ProjectManager.hpp"
#include "CXPM/Testing/TemporaryDirectory.hpp"
#include "CXPM/Testing/TestGroup.hpp"

#include <fstream>

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Controllers::Testing {

struct ProjectManagerTest : public TestGroup {
  ProjectManagerTest()
      : TestGroup(
            "Controllers::ProjectManager",
            {
                {"install_target() installs an executable's binary and .pc "
                 "file even when the target has no include directories "
                 "(regression: this used to silently install nothing)",
                 [](TestContext &ctx) {
                   auto project_dir =
                       temporary_directory("project-manager-install-no-headers");
                   auto prefix_dir =
                       temporary_directory("project-manager-install-no-headers-prefix");

                   auto previous_cwd = std::filesystem::current_path();
                   std::filesystem::current_path(project_dir);

                   std::ofstream("demo") << "#!/bin/sh\necho hi\n";

                   CXPM::TargetDescriptor target;
                   target.name = "demo";
                   target.type = "executable";
                   target.version = "1.0.0";
                   // deliberately empty: this is exactly the configuration that used to
                   // make install_target() install nothing at all.
                   target.include_directories = {};

                   CXPM::ToolchainDescriptor toolchain;

                   auto [status, installed_target, installed_toolchain] =
                       CXPM::Controllers::ProjectManager::install_target(
                           target, toolchain, String(prefix_dir.string()));
                   Utils::Unused{installed_target, installed_toolchain};

                   std::filesystem::current_path(previous_cwd);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.check(std::filesystem::exists(prefix_dir / "bin" / "demo"),
                             "expected the executable to be installed to <prefix>/bin");
                   ctx.check(
                       std::filesystem::exists(prefix_dir / "lib" / "pkgconfig" /
                                               "demo.pc"),
                       "expected a .pc file even with no include directories");

                   std::filesystem::remove_all(project_dir);
                   std::filesystem::remove_all(prefix_dir);
                 }},
                {"install_target() also copies headers from every include "
                 "directory",
                 [](TestContext &ctx) {
                   auto project_dir =
                       temporary_directory("project-manager-install-headers");
                   auto prefix_dir =
                       temporary_directory("project-manager-install-headers-prefix");

                   auto previous_cwd = std::filesystem::current_path();
                   std::filesystem::current_path(project_dir);

                   std::ofstream("demo") << "#!/bin/sh\necho hi\n";
                   std::filesystem::create_directories(project_dir / "include");
                   std::ofstream(project_dir / "include" / "demo.hpp")
                       << "#pragma once\n";
                   std::ofstream(project_dir / "include" / "ignored.txt")
                       << "not a header\n";

                   CXPM::TargetDescriptor target;
                   target.name = "demo";
                   target.type = "executable";
                   target.include_directories = {
                       (project_dir / "include").string()};

                   CXPM::ToolchainDescriptor toolchain;

                   auto [status, installed_target, installed_toolchain] =
                       CXPM::Controllers::ProjectManager::install_target(
                           target, toolchain, String(prefix_dir.string()));
                   Utils::Unused{installed_target, installed_toolchain};

                   std::filesystem::current_path(previous_cwd);

                   ctx.check(status == CXPM::Status::Success);
                   ctx.check(std::filesystem::exists(prefix_dir / "include" /
                                                     "demo" / "demo.hpp"),
                             "expected demo.hpp to be installed under "
                             "<prefix>/include/<target-name>");
                   ctx.check(!std::filesystem::exists(prefix_dir / "include" /
                                                      "demo" / "ignored.txt"),
                             "a non-header file should not have been installed");

                   std::filesystem::remove_all(project_dir);
                   std::filesystem::remove_all(prefix_dir);
                 }},
                {"clean() removes package.cpp.o next to the project (regression: "
                 "used to build \"<dir>/package.cpp/.o\" instead of "
                 "\"<dir>/package.cpp.o\", due to appending the source name and "
                 "the \".o\" suffix as two separate path components)",
                 [](TestContext &ctx) {
                   auto project_dir = temporary_directory("project-manager-clean");
                   std::ofstream(project_dir / "package.cpp.o") << "x";
                   std::ofstream(project_dir / "package.loader.cpp.o") << "x";
                   std::ofstream(project_dir / "package.loader.cpp") << "x";

                   CXPM::Controllers::ProjectManager::clean(
                       String(project_dir.string()), {});

                   ctx.check(!std::filesystem::exists(project_dir / "package.cpp.o"));
                   ctx.check(
                       !std::filesystem::exists(project_dir / "package.loader.cpp.o"));
                   ctx.check(
                       !std::filesystem::exists(project_dir / "package.loader.cpp"));

                   std::filesystem::remove_all(project_dir);
                 }},
                {"clean() removes generated toolchain loader/shared-object "
                 "files",
                 [](TestContext &ctx) {
                   auto project_dir =
                       temporary_directory("project-manager-clean-toolchains");
                   std::ofstream(project_dir / "toolchain-g++.loader.cpp") << "x";
                   std::ofstream(project_dir / "libtoolchain-g++.so") << "x";
                   std::ofstream(project_dir / "unrelated-file.txt") << "x";

                   CXPM::Controllers::ProjectManager::clean(
                       String(project_dir.string()), {});

                   ctx.check(!std::filesystem::exists(
                       project_dir / "toolchain-g++.loader.cpp"));
                   ctx.check(
                       !std::filesystem::exists(project_dir / "libtoolchain-g++.so"));
                   ctx.check(std::filesystem::exists(project_dir / "unrelated-file.txt"),
                             "clean() should not touch unrelated files");

                   std::filesystem::remove_all(project_dir);
                 }},
            }) {}
};

inline static ProjectManagerTest project_manager_test_instance;

} // namespace CXPM::Controllers::Testing
