#pragma once

#include <CXPM/Core/Threading/Mutex.hpp>
#include <CXPM/Core/Threading/UniqueLock.hpp>
#include <CXPM/Core/SharedPointer.hpp>

#include <memory>

class Object {
public:
  Object() : _mutex(SharedPointer<Mutex>(new Mutex())) {}
  virtual ~Object() {}

  std::shared_ptr<Mutex> mutex() {
    return _mutex;
  }

  UniqueLock<Mutex> acquire_lock() const {
    return UniqueLock<Mutex>(*_mutex);
  }
  private:
  SharedPointer<Mutex> _mutex;
};