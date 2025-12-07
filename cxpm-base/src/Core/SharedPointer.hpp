#pragma once

#include <memory>
#include <utility>

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

template <typename Tp>
struct EnableSharedFromThis : std::enable_shared_from_this<Tp> {
public:
  using std::enable_shared_from_this<Tp>::enable_shared_from_this;

  // SharedPointer<Tp> shared_from_this() {
  //   return SharedPointer<Tp>(shared_from_this());
  // }
};