#pragma once

#include "Modules/Serialization/Base/KeyValueTag.hpp"
#include <Core/Containers/String.hpp>


using namespace Core::Containers;

namespace Modules::Serialization::Base {

template <typename ValueType> struct ValueTag : KeyValueTag<ValueType> {
  ValueTag() {}
  ValueTag(const ValueType &value)
      : KeyValueTag<ValueType>(KeyValueTag<ValueType>::anonymous_name(),
                               value) {}
  virtual ~ValueTag() {}
};

} // namespace Modules::Serialization::Base