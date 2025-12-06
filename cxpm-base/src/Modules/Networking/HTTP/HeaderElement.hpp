#pragma once

#include "Core/Object.hpp"
#include <string>
#include <utility>

using namespace Core;

namespace Modules::Networking::HTTP {

class HeaderElement : public std::pair<std::string, std::string>,
                      public Object {
public:
  using std::pair<std::string, std::string>::pair;
};
} // namespace Modules::Networking::HTTP