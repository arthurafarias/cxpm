#pragma once

#include "Utils/Unused.hpp"
#include <atomic>
namespace Core::Globals {
static inline std::atomic_bool quit = false;
static inline void close(int signo) {
  Utils::Unused{signo};
  quit = true;
}
} // namespace Core::Globals