#pragma once

// These tests shell out to POSIX-guaranteed utilities (echo, false, /bin/sh via popen) rather
// than a C++ compiler, so they stay in the hermetic "unit" tier -- see testing.md's
// "Testability tiers" for why that distinction matters here.

#include "CXPM/Testing/TestGroup.hpp"
#include "CXPM/Utils/Unix/ShellManager.hpp"
#include "CXPM/Utils/Unused.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Utils::Unix::Testing {

struct ShellManagerTest : public TestGroup {
  ShellManagerTest()
      : TestGroup(
            "Utils::Unix::ShellManager",
            {
                {"exec() with dry=true never runs the command and returns "
                 "{0, \"\", \"\"}",
                 [](TestContext &ctx) {
                   auto [code, out, err] =
                       CXPM::Utils::Unix::ShellManager::exec(
                           "touch /nonexistent-directory/should-never-be-created",
                           true);
                   ctx.equal(code, 0);
                   ctx.equal(out, String(""));
                   ctx.equal(err, String(""));
                 }},
                {"exec() runs a real command and captures its trimmed stdout",
                 [](TestContext &ctx) {
                   auto [code, out, err] =
                       CXPM::Utils::Unix::ShellManager::exec(
                           "echo cxpm-shell-test");
                   Utils::Unused{err};
                   ctx.equal(code, 0);
                   ctx.equal(out, String("cxpm-shell-test"));
                 }},
                {"exec() propagates a non-zero exit code",
                 [](TestContext &ctx) {
                   auto [code, out, err] =
                       CXPM::Utils::Unix::ShellManager::exec("false");
                   Utils::Unused{out, err};
                   ctx.check(code != 0);
                 }},
                {"exec() with shell=true still runs the command successfully",
                 [](TestContext &ctx) {
                   auto [code, out, err] =
                       CXPM::Utils::Unix::ShellManager::exec(
                           "echo cxpm-shell-test-wrapped", false, true);
                   Utils::Unused{err};
                   ctx.equal(code, 0);
                   ctx.equal(out, String("cxpm-shell-test-wrapped"));
                 }},
                {"exec_async() runs the command on the shared ThreadPool and "
                 "fulfills its promise with the real result (regression: used "
                 "to pass its own `shell` argument into exec()'s `dry` "
                 "parameter, silently dry-running every async call)",
                 [](TestContext &ctx) {
                   auto promise = CXPM::Utils::Unix::ShellManager::exec_async(
                       "echo cxpm-async-test");
                   auto future = promise->get_future();
                   auto [code, out, err] = future.get();
                   Utils::Unused{err};
                   ctx.equal(code, 0);
                   ctx.equal(out, String("cxpm-async-test"));
                 }},
            }) {}
};

inline static ShellManagerTest shell_manager_test_instance;

} // namespace CXPM::Utils::Unix::Testing
