#pragma once

#include <atomic>
namespace Core::Globals {
static inline std::atomic_bool quit = false;
static inline void close(int signo) { quit = true; }
} // namespace Core::Globals