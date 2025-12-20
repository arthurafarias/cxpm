#pragma once

#include "Core/Containers/Map.hpp"
#include "Core/Containers/String.hpp"
#include "Modules/Serialization/JSON/JSONValue.hpp"
#include "Utils/Patterns/Creator.hpp"
#include <Modules/Streams/OutputStringStream.hpp>
#include <Modules/Testing/TestGroup.hpp>

using namespace Core::Containers;
using namespace Modules::Testing;
using namespace Modules::Serialization::Base;

namespace Modules { namespace Serialization { namespace JSON {
struct JSONValue;
// struct JSONObject : public Map<String, JSONValue> {
//   using Map<String, JSONValue>::Map;
//   using BaseType = Map<String, JSONValue>;
// };
using JSONObject = Map<String, JSONValue>;

namespace Testing {
struct JSONValueTest_BasicFunctionality_SerializeObject
    : public TestCase,
      public Utils::Patterns::Creator<
          JSONValueTest_BasicFunctionality_SerializeObject> {
  JSONValueTest_BasicFunctionality_SerializeObject()
      : TestCase("Serialize Value", "String Serialization Test") {}

  virtual void run() override {
    using namespace Modules::Streams;
    using namespace Modules::Serialization::Base;
    OutputStringStream os;
    // JSONOutputArchiver joa(os);
  }
};

struct JSONValueTest_BasicFunctionality : public TestGroup {
  JSONValueTest_BasicFunctionality()
      : TestGroup("JSON Value Test", "Basic Functionality") {
    tests = {JSONValueTest_BasicFunctionality_SerializeObject::create()};
  }
};
} // namespace Testing
} } } // namespace Modules::Serialization::JSON