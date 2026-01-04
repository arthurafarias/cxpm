#pragma once

#include "CXPM/Core/Containers/String.hpp"
#include "CXPM/Modules/Console/Colors/ANSIColorCode.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Modules::Console::Colors;

namespace CXPM::Modules::Console::Colors::Utils {
    inline constexpr String paint_string(const String &str, const String &color) {
  return color + str + ANSIColorCode::Reset;
}

} // namespace CXPM::Modules::Console::Colors