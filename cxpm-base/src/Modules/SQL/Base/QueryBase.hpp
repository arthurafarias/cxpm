#pragma once

#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Modules::SQL::Base {
struct QueryBase {
  virtual String compile() = 0;
};
} // namespace Modules::SQL::Base