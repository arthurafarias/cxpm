#pragma once

// Uses a locally-constructed ThreadPool, never CXPM::Threading::ThreadPool::get_instance(),
// so this test neither depends on nor perturbs the process-wide singleton other code
// (Utils::Unix::ShellManager::exec_async, exercised separately in ShellManagerTest.hpp) shares.

#include "CXPM/Core/Threading/ThreadPool.hpp"
#include "CXPM/Testing/TestGroup.hpp"

#include <atomic>
#include <chrono>
#include <thread>

using namespace CXPM::Testing;

namespace CXPM::Threading::Testing {

struct ThreadPoolTest : public TestGroup {
  ThreadPoolTest()
      : TestGroup(
            "Core::Threading::ThreadPool",
            {
                {"submit() runs the task on a worker thread",
                 [](TestContext &ctx) {
                   CXPM::Threading::ThreadPool pool(2);
                   std::atomic<bool> ran{false};

                   pool.submit([&ran] { ran = true; });

                   auto deadline = std::chrono::steady_clock::now() +
                                   std::chrono::seconds(5);
                   while (!ran.load() &&
                          std::chrono::steady_clock::now() < deadline) {
                     std::this_thread::sleep_for(std::chrono::milliseconds(1));
                   }

                   ctx.check(ran.load(), "submitted task did not run within 5s");
                 }},
                {"submit() runs multiple tasks, all of them",
                 [](TestContext &ctx) {
                   CXPM::Threading::ThreadPool pool(4);
                   std::atomic<int> completed{0};
                   constexpr int task_count = 16;

                   for (int i = 0; i < task_count; ++i) {
                     pool.submit([&completed] { ++completed; });
                   }

                   auto deadline = std::chrono::steady_clock::now() +
                                   std::chrono::seconds(5);
                   while (completed.load() < task_count &&
                          std::chrono::steady_clock::now() < deadline) {
                     std::this_thread::sleep_for(std::chrono::milliseconds(1));
                   }

                   ctx.equal(completed.load(), task_count);
                 }},
                {"an exception thrown by a task is caught rather than "
                 "terminating the pool",
                 [](TestContext &ctx) {
                   CXPM::Threading::ThreadPool pool(1);
                   std::atomic<bool> second_ran{false};

                   pool.submit([] { throw std::runtime_error("boom"); });
                   pool.submit([&second_ran] { second_ran = true; });

                   auto deadline = std::chrono::steady_clock::now() +
                                   std::chrono::seconds(5);
                   while (!second_ran.load() &&
                          std::chrono::steady_clock::now() < deadline) {
                     std::this_thread::sleep_for(std::chrono::milliseconds(1));
                   }

                   ctx.check(second_ran.load(),
                             "pool should keep processing tasks after one throws");
                 }},
            }) {}
};

inline static ThreadPoolTest thread_pool_test_instance;

} // namespace CXPM::Threading::Testing
