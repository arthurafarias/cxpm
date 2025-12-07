#pragma once

#include <Core/Object.hpp>
#include <Modules/Networking/TCP/BasicContext.hpp>

using namespace Core;

namespace Modules::Networking::TCP {

class ContextInterface : public Object, public BasicContext {
public:
  virtual ~ContextInterface() = default;
};
} // namespace Modules::Networking::TCP