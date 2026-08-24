#pragma once

// LoggerManager keeps its current stream/level in global static state with no reset API (see
// docs/SRS-unit-testing.md §8 and testing.md's "Testability tiers"). Every test case here
// redirects the stream to a *heap-allocated* std::ostringstream via a shared_ptr before logging
// anything -- never to a stack-local object -- because LoggerManager keeps its own shared_ptr to
// whatever stream was last set, and that reference outlives this test case. Pointing it at a
// stack object would leave a dangling pointer for every subsequent test in this same binary to
// crash on the next time anything logs.

#include "CXPM/Core/Logging/LoggerManager.hpp"
#include "CXPM/Testing/TestGroup.hpp"

#include <sstream>

using namespace CXPM::Testing;

namespace CXPM::Core::Logging::Testing {

struct LoggerManagerTest : public TestGroup {
  LoggerManagerTest()
      : TestGroup(
            "Core::Logging::LoggerManager",
            {
                {"info() writes the formatted message prefixed with INFO",
                 [](TestContext &ctx) {
                   auto sink = std::make_shared<std::ostringstream>();
                   CXPM::Core::Logging::LoggerManager::stream_set(sink);
                   CXPM::Core::Logging::LoggerManager::level_set(
                       CXPM::Core::Logging::LoggerManager::Level::Max);

                   CXPM::Core::Logging::LoggerManager::info("hello {}", 42);

                   auto text = sink->str();
                   ctx.check(text.find("INFO: hello 42") != std::string::npos,
                             "expected an INFO-prefixed, formatted line");
                 }},
                {"debug()/warning()/error() each use their own prefix",
                 [](TestContext &ctx) {
                   auto sink = std::make_shared<std::ostringstream>();
                   CXPM::Core::Logging::LoggerManager::stream_set(sink);
                   CXPM::Core::Logging::LoggerManager::level_set(
                       CXPM::Core::Logging::LoggerManager::Level::Max);

                   CXPM::Core::Logging::LoggerManager::debug("d");
                   CXPM::Core::Logging::LoggerManager::warning("w");
                   CXPM::Core::Logging::LoggerManager::error("e");

                   auto text = sink->str();
                   ctx.check(text.find("DEBUG: d") != std::string::npos);
                   ctx.check(text.find("WARNING: w") != std::string::npos);
                   ctx.check(text.find("ERROR: e") != std::string::npos);
                 }},
                {"level_set(Info) suppresses debug() and warning()",
                 [](TestContext &ctx) {
                   auto sink = std::make_shared<std::ostringstream>();
                   CXPM::Core::Logging::LoggerManager::stream_set(sink);
                   CXPM::Core::Logging::LoggerManager::level_set(
                       CXPM::Core::Logging::LoggerManager::Level::Info);

                   CXPM::Core::Logging::LoggerManager::debug("should not appear");
                   CXPM::Core::Logging::LoggerManager::warning("should not appear either");
                   CXPM::Core::Logging::LoggerManager::info("should appear");

                   auto text = sink->str();
                   ctx.check(text.find("should not appear") == std::string::npos,
                             "debug()/warning() should be suppressed below Level::Info");
                   ctx.check(text.find("should appear") != std::string::npos);

                   // Restore Max so later test groups aren't silently starved of log
                   // output if they ever come to depend on it.
                   CXPM::Core::Logging::LoggerManager::level_set(
                       CXPM::Core::Logging::LoggerManager::Level::Max);
                 }},
                {"error() is never suppressed regardless of level",
                 [](TestContext &ctx) {
                   auto sink = std::make_shared<std::ostringstream>();
                   CXPM::Core::Logging::LoggerManager::stream_set(sink);
                   CXPM::Core::Logging::LoggerManager::level_set(
                       CXPM::Core::Logging::LoggerManager::Level::Info);

                   CXPM::Core::Logging::LoggerManager::error("always shown");

                   ctx.check(sink->str().find("always shown") != std::string::npos);

                   CXPM::Core::Logging::LoggerManager::level_set(
                       CXPM::Core::Logging::LoggerManager::Level::Max);
                 }},
                {"stream_set(nullptr) is a no-op that keeps the previous stream",
                 [](TestContext &ctx) {
                   auto sink = std::make_shared<std::ostringstream>();
                   CXPM::Core::Logging::LoggerManager::stream_set(sink);
                   CXPM::Core::Logging::LoggerManager::stream_set(nullptr);
                   CXPM::Core::Logging::LoggerManager::level_set(
                       CXPM::Core::Logging::LoggerManager::Level::Max);

                   CXPM::Core::Logging::LoggerManager::info("still routed here");

                   ctx.check(sink->str().find("still routed here") !=
                             std::string::npos);
                 }},
            }) {}
};

inline static LoggerManagerTest logger_manager_test_instance;

} // namespace CXPM::Core::Logging::Testing
