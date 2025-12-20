#pragma once

#include <list>
#include <mutex>

#include <Core/Threading/ThreadPool.hpp>

namespace Core::Threading {
template <typename... ArgsTypes> class AnonymousSignal {
public:
  AnonymousSignal() = default;

  AnonymousSignal(AnonymousSignal &) = delete;
  AnonymousSignal &operator=(AnonymousSignal &) = delete;

  AnonymousSignal(AnonymousSignal &&) = delete;
  AnonymousSignal &operator=(AnonymousSignal &&) = delete;

  ~AnonymousSignal() { disconnect_all(); }

  using slot_type = Function<void(ArgsTypes...)>;
  using handle_type = typename std::list<slot_type>::iterator;

  const typename std::list<slot_type>::iterator connect(const slot_type &slot) {
    std::unique_lock<std::mutex> lock(_mutex);
    return _sinks.insert(_sinks.end(), std::move(slot));
  }

  void disconnect(const typename std::list<slot_type>::iterator &it) {
    std::unique_lock<std::mutex> lock(_mutex);
    _sinks.erase(it);
  }

  void disconnect_all() {
    std::unique_lock<std::mutex> lock(_mutex);
    _sinks.clear();
  }

  void operator()(ArgsTypes... args) {
    std::unique_lock<std::mutex> lock(_mutex);

    for (const auto &slot : _sinks) {
      slot(args...);
    }
  }

  AnonymousSignal &operator+=(const slot_type &slot) {
    connect(slot);
    return *this;
  }

  AnonymousSignal &operator-=(const typename std::list<slot_type>::iterator &it) {
    disconnect(it);
    return *this;
  }

  const auto &sinks() { return _sinks; }

private:
  std::mutex _mutex;
  std::list<slot_type> _sinks;
  ThreadPool &_ThreadPool = ThreadPool::get_instance();
};
} // namespace Core::Threading