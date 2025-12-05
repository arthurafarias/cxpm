#pragma once

#define UNUSED(x) ((void)(x))

namespace Utils {
struct Unused {
  template<typename ...UnusedTypes> 
  explicit Unused(const UnusedTypes &...) {}
  Unused(const Unused &) = delete;
  Unused &operator=(const Unused &) = delete;
  Unused(const Unused &&) = delete;
  Unused &operator=(const Unused &&) = delete;
};
} // namespace Utils