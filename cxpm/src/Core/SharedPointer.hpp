#pragma once

#include <memory>

template <typename... ArgsTypes>
class SharedPointer : public std::shared_ptr<ArgsTypes...> {
public:
  SharedPointer(std::shared_ptr<ArgsTypes...> ptr)
      : std::shared_ptr<ArgsTypes...>(ptr) {}
  using std::shared_ptr<ArgsTypes...>::shared_ptr;
};

template <typename ContainedType>
class EnableSharedFromThis
    : public std::enable_shared_from_this<ContainedType> {
public:
  using std::enable_shared_from_this<ContainedType>::enable_shared_from_this;

  template <typename... ConstructorArguments>
  inline static SharedPointer<ContainedType>
  make(const ConstructorArguments &&...args) {
    return SharedPointer<ContainedType>(new ContainedType(args...));
  }
};