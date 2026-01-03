#pragma once

#include <CXPM/Core/Functional/Function.hpp>
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
