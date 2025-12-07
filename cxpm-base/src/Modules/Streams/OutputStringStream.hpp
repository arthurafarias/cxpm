#pragma once

#include <ostream>
#include <sstream>

namespace Modules::Streams {
class OutputStringStream : public std::ostringstream {
public:
  using std::ostringstream::basic_ostream;
};
} // namespace Modules::Streams