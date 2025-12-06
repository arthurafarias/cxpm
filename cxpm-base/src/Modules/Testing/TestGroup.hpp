#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Functional/Function.hpp"
#include "Core/Logging/LoggerManager.hpp"
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

    Logging::LoggerManager::info("TestGroup: {}: Ended", name.c_str());
    for (auto test : tests) {
      Logging::LoggerManager::info("Test: {}: Started", test->name.c_str());
      try {
        test->run();
        Logging::LoggerManager::info("Test: {}: Ok", test->name.c_str());
      } catch (std::exception &ex) {
        Logging::LoggerManager::info("Test: {}: Failed", test->name.c_str());
        Logging::LoggerManager::info("Test: {}: Reason: {}", test->name.c_str(),
                                     ex.what());
      }
    }

    Logging::LoggerManager::info("TestGroup: {}: Ok", name.c_str());
  }
};
} // namespace Modules::Testing