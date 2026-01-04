#pragma once

#include "CXPM/Utils/Macros/StaticClass.hpp"
#include "CXPM/Modules/Graphics/Color.hpp"

using namespace CXPM::Modules::Graphics;

namespace CXPM::Modules::Console::Colors {
class DefaultPallete final {
  StaticClass(DefaultPallete);

public:
  const static inline auto White = Color{1, 1, 1, 1};
  const static inline auto Red = Color{1, 0, 0, 1};
  const static inline auto Green = Color{0, 1, 0, 1};
  const static inline auto Blue = Color{0, 0, 1, 1};
};
} // namespace CXPM::Modules::Console::Colors