#pragma once

#include "CXPM/Core/Patterns/Lockable.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Testing;

namespace CXPM::Core::Patterns::Testing {

struct LockableTest : public TestGroup {
  LockableTest()
      : TestGroup(
            "Core::Patterns::Lockable",
            {
                {"acquire_lock() returns a lock already owning the mutex",
                 [](TestContext &ctx) {
                   CXPM::Core::Patterns::Lockable lockable;
                   auto lock = lockable.acquire_lock();
                   ctx.check(lock.owns_lock());
                 }},
                {"a copy gets its own, different mutex instead of sharing "
                 "the original's (documented as a design flaw in "
                 "SRS-architecture.md item A25 -- this pins the *current* "
                 "behavior so a future fix has a test to update, not one "
                 "to silently break)",
                 [](TestContext &ctx) {
                   CXPM::Core::Patterns::Lockable original;
                   CXPM::Core::Patterns::Lockable copy = original;
                   ctx.check(original.mutex != copy.mutex);
                 }},
                {"copy-assignment likewise replaces the mutex rather than "
                 "sharing it",
                 [](TestContext &ctx) {
                   CXPM::Core::Patterns::Lockable original;
                   CXPM::Core::Patterns::Lockable other;
                   auto mutex_before_assignment = other.mutex;
                   other = original;
                   ctx.check(other.mutex != mutex_before_assignment);
                   ctx.check(other.mutex != original.mutex);
                 }},
            }) {}
};

inline static LockableTest lockable_test_instance;

} // namespace CXPM::Core::Patterns::Testing
