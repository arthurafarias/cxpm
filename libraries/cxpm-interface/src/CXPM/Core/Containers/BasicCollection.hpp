#pragma once

#include "CXPM/Core/Patterns/Lockable.hpp"
#include "CXPM/Modules/Serialization/ArrayEndToken.hpp"
#include "CXPM/Modules/Serialization/ArrayStartToken.hpp"
#include "CXPM/Modules/Serialization/SeparatorToken.hpp"
#include "CXPM/Modules/Serialization/ValueToken.hpp"
#include "CXPM/Utils/Unused.hpp"
#include <deque>
#include <functional>
#include <ranges>
#include <utility>

using namespace CXPM::Modules::Serialization;

namespace CXPM::Core::Containers {
template <typename ContainedType>
class BasicCollection : public std::deque<ContainedType>,
                        CXPM::Core::Patterns::Lockable {
public:
  template <typename... ArgsTypes>
  BasicCollection(const ArgsTypes &&...args)
      : std::deque<ContainedType>(std::forward<const ArgsTypes>(args)...) {}
  using std::deque<ContainedType>::deque;

  template <typename ReturnType>
  BasicCollection<ReturnType>
  transform(std::function<ReturnType(const ContainedType &)> transfomer) const {
    BasicCollection<ReturnType> retval;

    for (auto el : *this) {
      retval.push_back(transfomer(el));
    }

    return std::move(retval);
  }
};

template <typename Archiver, typename ValueType>
inline Archiver &
operator%(Archiver &ar, const ValueToken<BasicCollection<ValueType>> &token) {

  Utils::Unused{ar, token};

  auto &collection = token.value_get();

  ar % ArrayStartToken{"BasicCollection"};

  ar % ValueToken{collection.front()};

  for (auto& value :
       collection | std::views::drop(1)) {

    ar % SeparatorToken{};

    ar % ValueToken{value};
  }

  ar % ArrayEndToken{"BasicCollection"};

  return ar;
}

} // namespace CXPM::Core::Containers