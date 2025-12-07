#pragma once

#include "Core/Object.hpp"
#include <vector>
#include <functional>
#include <utility>

namespace Core::Containers {
template <typename ContainedType>
class ContiguousCollection : public std::vector<ContainedType>, public Object {
public:
  template <typename... ArgsTypes>
  ContiguousCollection(const ArgsTypes&&... args)
      : std::vector<ContainedType>(std::forward<const ArgsTypes>(args)...) {}
  using std::vector<ContainedType>::vector;

  template <typename ReturnType>
  ContiguousCollection<ReturnType>
  transform(std::function<ReturnType(const ContainedType &)> transfomer) const {
    ContiguousCollection<ReturnType> retval;

    for (auto el : *this) {
      retval.push_back(transfomer(el));
    }

    return std::move(retval);
  }


  // void append_range(const ContiguousCollection<ContainedType>& element) const {
  //   auto casted = static_cast<std::vector<ContainedType>>(element);
  //   append_range(casted);
  // }
};
} // namespace Core::Containers