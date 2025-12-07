#pragma once

#include "Core/Containers/Map.hpp"
#include "Core/Containers/RecursiveVariant.hpp"
#include "Core/Object.hpp"
#include "Modules/Testing/TestCase.hpp"
#include <map>

using namespace Core::Containers;

template <typename KeyType, typename... ArgsTypes> class RecursiveMap;

template <typename KeyType, typename... ArgsTypes>
class RecursiveMap
    : public Map<KeyType,
                 Variant<RecursiveMap<KeyType, ArgsTypes...>, ArgsTypes...>> {
public:
  using Map<KeyType,
            Variant<RecursiveMap<KeyType, ArgsTypes...>, ArgsTypes...>>::Map;
};