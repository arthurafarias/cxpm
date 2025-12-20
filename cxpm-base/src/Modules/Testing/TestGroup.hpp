#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Functional/Function.hpp"
#include "Core/Logging/Manager.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Testing/TestCase.hpp"
#include <exception>

using namespace Core;
using namespace Core::Functional;
using namespace Core::Containers;

namespace Modules::Testing {
struct TestGroup : public Object {
public:
  TestGroup(const String &name, const String &description)
      : name(name), description(description) {}
  String name;
  String description;
  Collection<SharedPointer<TestCase>> tests;

  void run() {
    auto lk = acquire_lock();

    Logging::Logger::info("TestGroup: {}: Ended", name.c_str());
    for (auto test : tests) {
      Logging::Logger::info("Test: {}: Started", test->name.c_str());
      try {
        test->run();
        Logging::Logger::info("Test: {}: Ok", test->name.c_str());
      } catch (std::exception &ex) {
        Logging::Logger::info("Test: {}: Failed", test->name.c_str());
        Logging::Logger::info("Test: {}: Reason: {}", test->name.c_str(),
                                     ex.what());
      }
    }

    Logging::Logger::info("TestGroup: {}: Ok", name.c_str());
  }
};
} // namespace Modules::Testing