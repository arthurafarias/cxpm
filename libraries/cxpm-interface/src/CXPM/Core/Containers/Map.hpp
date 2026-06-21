#pragma once

#include "CXPM/Modules/Serialization/ObjectEndToken.hpp"
#include "CXPM/Modules/Serialization/ObjectStartToken.hpp"
#include "CXPM/Modules/Serialization/PairToken.hpp"
#include "CXPM/Modules/Serialization/SeparatorToken.hpp"
#include "CXPM/Modules/Serialization/ValueToken.hpp"

#include "CXPM/Utils/Unused.hpp"

#include <map>
#include <ranges>

using namespace CXPM::Modules::Serialization;

namespace CXPM::Core::Containers {
template <typename... ArgsTypes> class Map : public std::map<ArgsTypes...> {
public:
  using std::map<ArgsTypes...>::map;
};

template <typename Archiver, typename KeyType, typename ValueType>
inline Archiver &operator%(Archiver &ar,
                           const ValueToken<Map<KeyType, ValueType>> &token) {

  auto &collection = token.value_get();

  Utils::Unused{ar, collection};

  ar % ObjectStartToken{"Map"};

  ar % PairToken{collection.front().first, collection.front().second};

  for (const auto &[key, value] :
       collection | std::views::drop(1) | std::views::enumerate) {

    ar % SeparatorToken{};

    ar % PairToken{key, value};
  }

  ar.stream_get() % ObjectEndToken{"Map"};

  return ar;
}

} // namespace CXPM::Core::Containers