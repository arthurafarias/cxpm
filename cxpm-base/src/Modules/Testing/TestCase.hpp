#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Functional/Function.hpp"

using namespace Core;
using namespace Core::Functional;
using namespace Core::Containers;

namespace Modules::Testing {
struct TestCase : public Object {
public:
  TestCase(const String &name, const String &description)
      : name(name), description(description) {}
  String name;
  String description;
  virtual void run() = 0;
};
} // namespace Modules::Testing