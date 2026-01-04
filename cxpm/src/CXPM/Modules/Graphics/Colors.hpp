#pragma once

#include "CXPM/Utils/Macros/StaticClass.hpp"
#include <CXPM/Modules/Graphics/Color.hpp>

namespace CXPM::Modules::Graphics {
class Colors final {
  StaticClass(Colors);

public:
  inline static const auto Red = Color{1, 0, 0, 1};
  inline static const auto Green = Color{0, 1, 0, 1};
  inline static const auto Blue = Color{0, 0, 1, 1};
};
} // namespace CXPM::Core::Graphics