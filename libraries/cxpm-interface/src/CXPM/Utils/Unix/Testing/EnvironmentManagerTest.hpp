#pragma once

#include "CXPM/Testing/TemporaryDirectory.hpp"
#include "CXPM/Testing/TestGroup.hpp"
#include "CXPM/Utils/Unix/EnvironmentManager.hpp"

#include <cstdlib>
#include <fstream>

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Utils::Unix::Testing {

namespace {

// Saves and restores an environment variable around a test case so env mutation in one test
// never leaks into another (there is no reset API on EnvironmentManager itself to rely on).
struct ScopedEnv {
  ScopedEnv(const char *name, const char *value) : name_(name) {
    const char *previous = std::getenv(name);
    had_previous_ = previous != nullptr;
    if (had_previous_) {
      previous_value_ = previous;
    }
    if (value == nullptr) {
      ::unsetenv(name);
    } else {
      ::setenv(name, value, 1);
    }
  }

  ~ScopedEnv() {
    if (had_previous_) {
      ::setenv(name_, previous_value_.c_str(), 1);
    } else {
      ::unsetenv(name_);
    }
  }

  ScopedEnv(const ScopedEnv &) = delete;
  ScopedEnv &operator=(const ScopedEnv &) = delete;

private:
  const char *name_;
  bool had_previous_;
  std::string previous_value_;
};

} // namespace

struct EnvironmentManagerTest : public TestGroup {
  EnvironmentManagerTest()
      : TestGroup(
            "Utils::Unix::EnvironmentManager",
            {
                {"get() on an unset variable returns an empty collection",
                 [](TestContext &ctx) {
                   ScopedEnv guard("CXPM_TEST_UNSET_VAR", nullptr);
                   auto result = CXPM::Utils::Unix::EnvironmentManager::get(
                       "CXPM_TEST_UNSET_VAR");
                   ctx.check(result.empty());
                 }},
                {"get() splits on ':' and ';'",
                 [](TestContext &ctx) {
                   ScopedEnv guard("CXPM_TEST_SPLIT_VAR", "a:b;c");
                   auto result = CXPM::Utils::Unix::EnvironmentManager::get(
                       "CXPM_TEST_SPLIT_VAR");
                   ctx.equal(result.size(), std::size_t{3});
                   ctx.equal(result[0], String("a"));
                   ctx.equal(result[1], String("b"));
                   ctx.equal(result[2], String("c"));
                 }},
                {"get() on a single-token variable returns one element",
                 [](TestContext &ctx) {
                   ScopedEnv guard("CXPM_TEST_SINGLE_VAR", "solo");
                   auto result = CXPM::Utils::Unix::EnvironmentManager::get(
                       "CXPM_TEST_SINGLE_VAR");
                   ctx.equal(result.size(), std::size_t{1});
                   ctx.equal(result[0], String("solo"));
                 }},
                {"which() on an existing absolute path returns that same path",
                 [](TestContext &ctx) {
                   auto directory = temporary_directory("environment-manager-which");
                   auto file_path = directory / "some-file";
                   std::ofstream(file_path) << "x";

                   auto result = CXPM::Utils::Unix::EnvironmentManager::which(
                       file_path.string().c_str());
                   ctx.equal(result, String(file_path.string()));

                   std::filesystem::remove_all(directory);
                 }},
                {"which() on a nonexistent absolute path returns an empty string",
                 [](TestContext &ctx) {
                   auto result = CXPM::Utils::Unix::EnvironmentManager::which(
                       "/nonexistent-cxpm-test-binary");
                   ctx.equal(result, String(""));
                 }},
                {"which() finds a non-absolute name on a PATH directory",
                 [](TestContext &ctx) {
                   auto directory = temporary_directory("environment-manager-which-path");
                   auto file_path = directory / "cxpm-test-tool";
                   std::ofstream(file_path) << "x";

                   ScopedEnv guard("PATH", directory.string().c_str());
                   auto result = CXPM::Utils::Unix::EnvironmentManager::which(
                       "cxpm-test-tool");
                   ctx.equal(result, String(file_path.string()));

                   std::filesystem::remove_all(directory);
                 }},
                {"which() returns an empty string when nothing on PATH matches",
                 [](TestContext &ctx) {
                   auto directory =
                       temporary_directory("environment-manager-which-path-miss");

                   ScopedEnv guard("PATH", directory.string().c_str());
                   auto result = CXPM::Utils::Unix::EnvironmentManager::which(
                       "cxpm-test-tool-that-does-not-exist");
                   ctx.equal(result, String(""));

                   std::filesystem::remove_all(directory);
                 }},
            }) {}
};

inline static EnvironmentManagerTest environment_manager_test_instance;

} // namespace CXPM::Utils::Unix::Testing
