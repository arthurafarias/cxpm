#pragma once

#include <memory>
#include <mutex>
namespace CXPM::Core::Patterns {
struct Lockable {

  Lockable() : mutex(std::make_shared<std::mutex>()) {}

  Lockable(const Lockable &) : mutex(std::make_shared<std::mutex>()) {}
  
  Lockable &operator=(const Lockable &) {
    this->mutex = std::make_shared<std::mutex>();
    return *this;
  }

  virtual std::unique_lock<std::mutex> acquire_lock() {
    return std::unique_lock<std::mutex>(*mutex);
  }

  std::shared_ptr<std::mutex> mutex = nullptr;
};
} // namespace CXPM::Core::Patterns