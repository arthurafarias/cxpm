#pragma once

#include "CXPM/Core/Containers/Object.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Core::Containers::Testing {

struct ObjectTest : public TestGroup {
  ObjectTest()
      : TestGroup(
            "Object",
            {
                {"behaves like a Map<String, Value>: insertion and lookup",
                 [](TestContext &ctx) {
                   Object object;
                   object["name"] = Value(String("cxpm"));
                   object["count"] = Value(3);

                   ctx.check(std::holds_alternative<String>(object.at("name")));
                   ctx.equal(std::get<String>(object.at("name")), String("cxpm"));
                   ctx.equal(std::get<int>(object.at("count")), 3);
                 }},
                {"also exposes Lockable's acquire_lock()",
                 [](TestContext &ctx) {
                   Object object;
                   auto lock = object.acquire_lock();
                   ctx.check(lock.owns_lock());
                 }},
            }) {}
};

inline static ObjectTest object_test_instance;

} // namespace CXPM::Core::Containers::Testing
