#ifndef _lt_networking_tcp_context_hpp_
#define _lt_networking_tcp_context_hpp_

#include "Modules/Networking/TCP/ContextInterface.hpp"
#include <Core/Object.hpp>

namespace Modules::Networking::TCP {

using namespace Core;

class DefaultContext : public ContextInterface {
public:
  DefaultContext(int param = 0) {}
};

} // namespace Modules::Networking::TCP

#endif