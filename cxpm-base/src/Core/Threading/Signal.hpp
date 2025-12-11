/***********************************************************************************************************************
 *                                                                                                                      *
 * Threaded Signals v0.1 *
 *                                                                                                                      *
 * Copyright (c) 2015-(today) Arthur de A. Farias * All rights reserved. *
 *                                                                                                                      *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the     * following conditions are
 *met: *
 *                                                                                                                      *
 *    * Redistributions of source code must retain the above copyright notice,
 *this list of conditions, and the         * following disclaimer. *
 *    * Redistributions in binary form must reproduce the above copyright
 *notice, this list of conditions and the       * following disclaimer in the
 *documentation and/or other materials provided with the distribution. *
 *    * Neither the name of the author nor the names of any contributors may be
 *used to endorse or promote products     * derived from this software without
 *specific prior written permission.                                           *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED
 *WARRANTIES, INCLUDING, BUT NOT LIMITED   * TO, THE IMPLIED WARRANTIES OF
 *MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 *EVENT SHALL * THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        * (INCLUDING, BUT NOT
 *LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *PROFITS; OR       * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT * (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *EVEN IF ADVISED OF THE       * POSSIBILITY OF SUCH DAMAGE. *
 *                                                                                                                      *
 ***********************************************************************************************************************/

/**
        @file signal.hpp
        @author Arthur de A. Farias <arthur@afarias.org>
        @brief Signal Class Declaration/Definition
 */

#ifndef _threaded_signals_hpp_
#define _threaded_signals_hpp_

#include "Core/Threading/Mutex.hpp"
#include "Core/Threading/UniqueLock.hpp"
#include <functional>
#include <list>
#include <mutex>

#include <Core/Logging/LoggerManager.hpp>
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
  using HandleType = std::list<SlotType>::iterator;

  const std::list<SlotType>::iterator connect(const SlotType &slot) {
    UniqueLock<Mutex> lock(_mutex);

    Core::Logging::LoggerManager::debug("Core::Threading::signal", __func__);

    return _sinks.insert(_sinks.end(), std::move(slot));
  }

  void disconnect(const std::list<SlotType>::iterator &it) {
    std::unique_lock<std::mutex> lock(_mutex);

    Core::Logging::LoggerManager::debug("{}: {}", __func__, "");

    _sinks.erase(it);
  }

  void disconnect_all() {
    std::unique_lock<std::mutex> lock(_mutex);

    Core::Logging::LoggerManager::debug("{}: {}", __func__, "");

    _sinks.clear();
  }

  void operator()(sender_type sender, args_types... args) {
    std::unique_lock<std::mutex> lock(_mutex);

    Core::Logging::LoggerManager::debug("{}: {}", __func__, "call slot");

    for (const auto &slot : _sinks) {

      Core::Logging::LoggerManager::debug("{}: {}", __func__, "submit slot call");

      _thread_pool.submit([slot, sender, args...]() {
        Core::Logging::LoggerManager::debug("{}: {}", __func__, "begin slot call");

        slot(sender, args...);

        Core::Logging::LoggerManager::debug("{}: {}", __func__,
                                            "end slot call");
      });
    }
  }

  Signal &operator+=(const SlotType &slot) {
    connect(slot);
    return *this;
  }

  Signal &operator-=(const std::list<SlotType>::iterator &it) {
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