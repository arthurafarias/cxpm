#pragma once

#include <Core/SharedPointer.hpp>

template <typename Derived> class Iterator {
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = Derived;
  using difference_type = std::ptrdiff_t;
  using pointer = SharedPointer<Derived>;
  using reference = Derived &;

  Iterator<Derived> &operator++() {
    ptr = ptr->next();
    return *this;
  }

  Iterator<Derived> &operator++(int) {
    ptr = ptr->next();
    return *this;
  }

  Derived &operator*() { return *ptr; }

  SharedPointer<Derived> operator->() { return ptr; }

  bool operator==(const Iterator &rhs) { return ptr == rhs.ptr; }

  bool operator!=(const Iterator &rhs) { return ptr != rhs.ptr; }

private:
  SharedPointer<Derived> ptr;
};