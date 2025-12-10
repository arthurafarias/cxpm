#pragma once

#include <memory>
#include <utility>

#include <Core/WeakPointer.hpp>

template <typename PointerType>
class SharedPointer : public std::shared_ptr<PointerType> {
public:
  SharedPointer(std::shared_ptr<PointerType> ptr)
      : std::shared_ptr<PointerType>(ptr) {}
  using std::shared_ptr<PointerType>::shared_ptr;

  template <typename... PointerTypeConstructorArgumentsTypes>
  inline static SharedPointer<PointerType>
  make(PointerTypeConstructorArgumentsTypes... args) {
    return SharedPointer<PointerType>(new PointerType(
        std::forward<PointerTypeConstructorArgumentsTypes>(args)...));
  }
};

template <typename PointerType>
class EnableSharedFromThis : public std::enable_shared_from_this<PointerType> {
public:
  using std::enable_shared_from_this<PointerType>::enable_shared_from_this;
};