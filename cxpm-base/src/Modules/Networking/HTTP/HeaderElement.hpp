#pragma once

#include "Core/Object.hpp"
#include "Core/Containers/String.hpp"

#include <string>
#include <utility>

using namespace Core;
using namespace Core::Containers;

namespace Modules::Networking::HTTP {

class HeaderElement : public std::pair<String, String>,
                      public Object {
public:
  using std::pair<String, String>::pair;
};
} // namespace Modules::Networking::HTTP