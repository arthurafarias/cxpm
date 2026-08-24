#pragma once

// ToolchainManager::valid()/by_name()/autoselect()/add() are pure/deterministic once a
// ToolchainDescriptor already exists in hand -- what needs a real compiler is *producing* that
// descriptor from a toolchain.cpp (autoscan/build_toolchain_plugin/load_toolchain_plugin), which
// stays in the "Hard" testability tier and is covered only by the opt-in CLI integration tests
// (see testing.md). valid() only ever checks *existence* of the configured compiler/linker
// paths and directories -- not that they're actually invocable -- so a fixture file stands in
// for "a compiler" perfectly well here.
//
// add()/by_name() mutate ToolchainManager's shared static toolchain registry, which has no
// reset API (see docs/SRS-architecture.md item B29/A17-adjacent testability note). Every
// descriptor added below uses a name unique to this file so it can never collide with anything
// another test group might add.

#include "CXPM/Testing/TemporaryDirectory.hpp"
#include "CXPM/Testing/TestGroup.hpp"
#include "CXPM/ToolchainManager.hpp"

#include <fstream>

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Controllers::Testing {

struct ToolchainManagerTest : public TestGroup {
  ToolchainManagerTest()
      : TestGroup(
            "Controllers::ToolchainManager",
            {
                {"valid() rejects a descriptor with an empty name",
                 [](TestContext &ctx) {
                   CXPM::ToolchainDescriptor descriptor;
                   descriptor.name = "";
                   ctx.check(!CXPM::Controllers::ToolchainManager::valid(descriptor));
                 }},
                {"valid() rejects a descriptor whose compiler/linker "
                 "executables don't exist",
                 [](TestContext &ctx) {
                   CXPM::ToolchainDescriptor descriptor;
                   descriptor.name = "cxpm-test-missing-compiler";
                   descriptor.compiler_executable =
                       "/nonexistent-cxpm-test-compiler";
                   descriptor.linker_executable =
                       "/nonexistent-cxpm-test-linker";
                   ctx.check(!CXPM::Controllers::ToolchainManager::valid(descriptor));
                 }},
                {"valid() rejects a descriptor whose link/include directories "
                 "don't exist",
                 [](TestContext &ctx) {
                   auto directory =
                       temporary_directory("toolchain-manager-valid-dirs");
                   auto compiler_path = directory / "cc";
                   std::ofstream(compiler_path) << "x";

                   CXPM::ToolchainDescriptor descriptor;
                   descriptor.name = "cxpm-test-missing-dirs";
                   descriptor.compiler_executable = compiler_path.string();
                   descriptor.linker_executable = compiler_path.string();
                   descriptor.include_directories = {"/nonexistent-include-dir"};

                   ctx.check(!CXPM::Controllers::ToolchainManager::valid(descriptor));

                   std::filesystem::remove_all(directory);
                 }},
                {"valid() accepts a descriptor whose compiler/linker/"
                 "directories all exist",
                 [](TestContext &ctx) {
                   auto directory =
                       temporary_directory("toolchain-manager-valid-ok");
                   auto compiler_path = directory / "cc";
                   std::ofstream(compiler_path) << "x";

                   CXPM::ToolchainDescriptor descriptor;
                   descriptor.name = "cxpm-test-valid-toolchain";
                   descriptor.compiler_executable = compiler_path.string();
                   descriptor.linker_executable = compiler_path.string();
                   descriptor.include_directories = {directory.string()};
                   descriptor.link_directories = {directory.string()};

                   ctx.check(CXPM::Controllers::ToolchainManager::valid(descriptor));

                   std::filesystem::remove_all(directory);
                 }},
                {"add() then by_name() finds the descriptor by exact name",
                 [](TestContext &ctx) {
                   CXPM::ToolchainDescriptor descriptor;
                   descriptor.name = "cxpm-test-by-name-toolchain";
                   descriptor.language = "cxpm-test-language";
                   CXPM::Controllers::ToolchainManager::add(descriptor);

                   auto found = CXPM::Controllers::ToolchainManager::by_name(
                       "cxpm-test-by-name-toolchain");
                   ctx.equal(found.name, String("cxpm-test-by-name-toolchain"));
                 }},
                {"by_name() throws for a name that was never added",
                 [](TestContext &ctx) {
                   ctx.throws<CXPM::Core::Exceptions::RuntimeException>([] {
                     CXPM::Controllers::ToolchainManager::by_name(
                         "cxpm-test-toolchain-that-was-never-added");
                   });
                 }},
                {"autoselect() finds the first toolchain matching the "
                 "target's language",
                 [](TestContext &ctx) {
                   CXPM::ToolchainDescriptor descriptor;
                   descriptor.name = "cxpm-test-autoselect-toolchain";
                   descriptor.language = "cxpm-test-autoselect-language";
                   CXPM::Controllers::ToolchainManager::add(descriptor);

                   CXPM::TargetDescriptor target;
                   target.language = "cxpm-test-autoselect-language";

                   auto found =
                       CXPM::Controllers::ToolchainManager::autoselect(target);
                   ctx.equal(found.name,
                             String("cxpm-test-autoselect-toolchain"));
                 }},
                {"autoselect() throws when no toolchain matches the "
                 "target's language",
                 [](TestContext &ctx) {
                   CXPM::TargetDescriptor target;
                   target.language = "cxpm-test-language-nothing-supports";
                   ctx.throws<CXPM::Core::Exceptions::RuntimeException>(
                       [&target] {
                         CXPM::Controllers::ToolchainManager::autoselect(target);
                       });
                 }},
            }) {}
};

inline static ToolchainManagerTest toolchain_manager_test_instance;

} // namespace CXPM::Controllers::Testing
