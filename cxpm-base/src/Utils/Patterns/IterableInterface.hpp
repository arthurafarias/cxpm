#pragma once

template <typename Derived> class IterableInterface : public Core::Object {
public:
  virtual SharedPointer<Derived> next(int offset) = 0;
  virtual SharedPointer<Derived> previous(int offset) = 0;
};