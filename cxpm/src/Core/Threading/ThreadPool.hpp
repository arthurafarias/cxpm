/***********************************************************************************************************************
 *                                                                                                                      *
 * Threaded Signals v0.1 *
 *                                                                                                                      *
 * Copyright (c) 2015-(today) Arthur de A. Farias * All rights reserved. *
 *                                                                                                                      *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the     * following conditions are
 *met:                                                                                                                  *
 *                                                                                                                      *
 *    * Redistributions of source code must retain the above copyright notice,
 *this list of conditions, and the         * following disclaimer.                                    *
 *                                                                                                                      *
 *    * Redistributions in binary form must reproduce the above copyright
 *notice, this list of conditions and the       * following disclaimer in the
 *documentation and/or other materials provided with the distribution. *
 *                                                                                                                      *
 *    * Neither the name of the author nor the names of any contributors may be
 *used to endorse or promote products     * derived from this software without
 *specific prior written permission.                                           *
 *                                                                                                                      *
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
  @brief Thread Pool Declaration/Definition
 */

#ifndef _Threading_ThreadPool_hpp_
#define _Threading_ThreadPool_hpp_

#include "Core/Functional/Function.hpp"
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>


namespace Threading {

class ThreadPool
{

  public:

  using task_type = Function<void()>;

  ThreadPool(int hardware_concurrency = std::thread::hardware_concurrency())
  {
    /** push task evaluator (task_launcher in the thread pool) */
    for (int i = 0; i < hardware_concurrency; i++) {
      _pool.emplace_back(std::bind(&ThreadPool::task_launcher, this));
    }
  }

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool operator=(const ThreadPool&) = delete;

  ThreadPool(const ThreadPool&&) = delete;
  ThreadPool operator=(const ThreadPool&&) = delete;

  void submit(task_type t)
  {
    std::unique_lock<std::mutex> lk{ _mutex };
    _q.push(t);
    _q_cond.notify_one();
  }

  static ThreadPool& get_instance()
  {
    static ThreadPool _ThreadPool;
    return _ThreadPool;
  }

  void shutdown()
  {
    _stop = true;
    _q_cond.notify_all();
    for (auto& t : _pool)
      t.join();
  }

  virtual ~ThreadPool() { shutdown(); }

protected:
  void task_launcher()
  {
    while (!_stop) {

      Function<void()> task;

      {
        auto lock = std::unique_lock<std::mutex>{ _mutex };
        _q_cond.wait(lock, [this] { return !_q.empty() || _stop; });

        if (_stop && _q.empty())
          break;

        task = std::move(_q.front());
        _q.pop();
      }

      if (task) {
        try {
          task();
        } catch (const std::exception& e) {
          std::cerr << e.what() << std::endl;
        } catch (...) {
          // Catch-all for safety
        }
      }
    }
  }

private:
  std::queue<Function<void()>> _q;
  std::condition_variable _q_cond;
  std::vector<std::thread> _pool;
  std::mutex _mutex;
  std::atomic_bool _stop = false;
};
}

#endif