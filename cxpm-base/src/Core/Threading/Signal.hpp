#ifndef _threaded_signals_hpp_
#define _threaded_signals_hpp_

#include "Core/SharedPointer.hpp"
#include "Core/Threading/Mutex.hpp"
#include "Core/Threading/UniqueLock.hpp"
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>

#include <Core/Logging/Manager.hpp>
#include <Core/Threading/ThreadPool.hpp>

namespace Core::Threading {

template <typename sender_type, typename... args_types> class Signal {
public:
  Signal() = default;

  Signal(Signal &) = delete;
  Signal &operator=(Signal &) = delete;

  Signal(Signal &&) = delete;
  Signal &operator=(Signal &&) = delete;

  ~Signal() { disconnect_all(); }

  using SlotType = std::function<void(sender_type, args_types...)>;
  using HandleType = typename std::list<SlotType>::iterator;

  const typename std::list<SlotType>::iterator connect(const SlotType &slot) {
    UniqueLock<Mutex> lock(_mutex);

    Core::Logging::Logger::debug("Core::Threading::signal", __func__);

    return _sinks.insert(_sinks.end(), std::move(slot));
  }

  void disconnect(const typename std::list<SlotType>::iterator &it) {
    std::unique_lock<std::mutex> lock(_mutex);

    Core::Logging::Logger::debug("{}: {}", __func__, "");

    _sinks.erase(it);
  }

  void disconnect_all() {
    std::unique_lock<std::mutex> lock(_mutex);
    Core::Logging::Logger::debug("{}: {}", __func__, "");
    _sinks.clear();
  }

  std::vector<SharedPointer<std::promise<bool>>>
  operator()(sender_type sender, args_types... args) {
    std::unique_lock<std::mutex> lock(_mutex);

    Core::Logging::Logger::debug("{}: {}", __func__, "call slot");

    std::vector<SharedPointer<std::promise<bool>>> promises;

    for (const auto &slot : _sinks) {

      auto promise = std::make_shared<std::promise<bool>>();
      promises.push_back(promise);

      Core::Logging::Logger::debug("{}: {}", __func__,
                                          "submit slot call");

      _thread_pool.submit([slot, sender, promise, args...]() {
        Core::Logging::Logger::debug("{}: {}", __func__,
                                            "begin slot call");

        slot(sender, args...);

        Core::Logging::Logger::debug("{}: {}", __func__,
                                            "end slot call");

        promise->set_value(true);
      });
    }
    return promises;
  }

  Signal &operator+=(const SlotType &slot) {
    connect(slot);
    return *this;
  }

  Signal &operator-=(const typename std::list<SlotType>::iterator &it) {
    disconnect(it);
    return *this;
  }

  const auto &sinks() { return _sinks; }

protected:
  std::mutex _mutex;
  std::list<SlotType> _sinks;
  ThreadPool &_thread_pool = ThreadPool::get_instance();
};
} // namespace Core::Threading

#endif