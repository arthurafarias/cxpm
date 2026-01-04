#pragma once

#include "CXPM/Utils/Macros/StaticClass.hpp"
namespace CXPM::Modules::Console::Colors {
class ANSIColorCode final {
  StaticClass(ANSIColorCode);

public:
  static inline constexpr auto Black = "\033[30m";
  static inline constexpr auto Red = "\033[31m";
  static inline constexpr auto Green = "\033[32m";
  static inline constexpr auto Yellow = "\033[33m";
  static inline constexpr auto Blue = "\033[34m";
  static inline constexpr auto Reset = "\033[m";
};
} // namespace CXPM::Modules::Console::Color