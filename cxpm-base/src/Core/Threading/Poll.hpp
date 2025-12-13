#pragma once

#include "Core/Object.hpp"
#include "Utils/Patterns/Creator.hpp"
#include <functional>
#include <thread>

#include <Core/Functional/Function.hpp>

using namespace Core::Functional;

namespace Core::Threading {
class Poll : public Core::Object, public Utils::Patterns::Creator<Poll> {
public:
  template <typename F, typename... Args>
  Poll(F &&fn, Args &&...args) : _quit(false) {
    // Make copies/moves so the thread owns what it will call
    auto bound =
        std::bind(std::forward<F>(fn), this, std::forward<Args>(args)...);

    _thread = std::thread([this, fn = std::move(bound)]() mutable {
      while (!_quit.load(std::memory_order_relaxed)) {
        fn();
      }
    });
  }

  bool joinable() { return _thread.joinable(); }

  void join() { _thread.join(); }

  void quit() { _quit.store(true, std::memory_order_relaxed); }

  virtual ~Poll() {
    if (_thread.joinable())
      _thread.join(); // safe
  }

private:
  std::atomic<bool> _quit;
  std::thread _thread;
};
} // namespace Core::Threading