#pragma once

#include "CXPM/Core/Containers/Map.hpp"
#include "CXPM/Core/Containers/String.hpp"
#include "CXPM/Modules/Console/Colors/ANSIColorCode.hpp"
#include "CXPM/Modules/Console/Colors/DefaultPallete.hpp"
#include "CXPM/Modules/Graphics/Color.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Modules::Graphics;
using namespace CXPM::Modules::Console::Colors;

namespace CXPM::Modules::Console::Colors::Utils {

    inline constexpr String paint_string(const String &str, const String &color) {
  return color + str + ANSIColorCode::Reset;
}

} // namespace CXPM::Modules::Console::Colors