#pragma once

#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::Serialization {

class AbstractInputArchiver {
  AbstractInputArchiver(std::istream &stream) : stream(stream) {}

protected:
  virtual ~AbstractInputArchiver() {}

  virtual std::istream &stream_get() { return stream; }

private:
  std::istream &stream;
};
} // namespace CXPM::Modules::Serialization