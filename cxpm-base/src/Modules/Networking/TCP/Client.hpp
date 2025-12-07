#ifndef _lt_networking_tcp_client_hpp_
#define _lt_networking_tcp_client_hpp_

#include <Core/Threading/Signal.hpp>
#include <Modules/Networking/TCP/Socket.hpp>

namespace Modules::Networking::TCP {

using namespace Core;
using namespace Core::Threading;

class Client : public Socket
{
public:
  Client(SharedPointer<BasicContext> context = nullptr, int fd = -1)
    : Socket(context, fd)
  {
  }
};

}

#endif