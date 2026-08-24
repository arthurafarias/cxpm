#pragma once

#include "CXPM/Modules/Serialization/AbstractArchiver.hpp"
#include <CXPM/Core/Containers/String.hpp>

#include <istream>

using namespace CXPM::Core::Containers;

namespace CXPM::Modules::Serialization {

struct AbstractInputArchiver : public virtual AbstractArchiver {
  explicit AbstractInputArchiver(std::istream &stream) : stream(stream) {}

  virtual std::istream &stream_get() { return stream; }

protected:
  virtual ~AbstractInputArchiver() {}

private:
  std::istream &stream;
};
} // namespace CXPM::Modules::Serialization
