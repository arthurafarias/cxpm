#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Containers/Variant.hpp"
#include "Core/SharedPointer.hpp"
#include "Models/CompilerCommandDescriptor.hpp"
#include "Modules/Serialization/Base/AbstractArchiver.hpp"
#include "Modules/Serialization/Base/TagBase.hpp"
#include "Modules/Serialization/Base/ValueTag.hpp"

#include <Core/Containers/String.hpp>
#include <cstddef>
#include <type_traits>
#include <typeinfo>

using namespace Core::Containers;
using namespace Modules::Serialization::Base;

namespace Modules::Templating {

class MustacheLite : public ArchiveTagFactory {
public:
  MustacheLite(const String &view) : view(view) {}

  virtual String render() { return String(); }

private:
  String view;
  Map<String, SharedPointer<TagBase>> model;
};

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<ArrayTag> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<ObjectTag> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar, SharedPointer<bool> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<ValueTag<std::nullptr_t>> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<ValueTag<double>> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<ValueTag<int>> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<ValueTag<String>> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<KeyValueTag<bool>> tag) {
  return ar;
}

MustacheLite constexpr &
operator%(MustacheLite &ar, SharedPointer<KeyValueTag<std::nullptr_t>> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<KeyValueTag<double>> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<KeyValueTag<int>> tag) {
  return ar;
}

MustacheLite constexpr &operator%(MustacheLite &ar,
                                  SharedPointer<KeyValueTag<String>> tag) {
  return ar;
}

template <typename ContainedType>
MustacheLite constexpr &
operator%(MustacheLite &ar,
          SharedPointer<KeyValueTag<Collection<ContainedType>>> tag) {
  return ar;
}

class ValueType;
class ValueType
    : public Variant<std::nullptr_t, bool, int, double, String,
                     Map<String, ValueType>, Collection<ValueType>> {
  using BaseType = Variant<std::nullptr_t, bool, int, double, String,
                           Map<String, ValueType>, Collection<ValueType>>;
  using Variant<std::nullptr_t, bool, int, double, String,
                Map<String, ValueType>, Collection<ValueType>>::Variant;
};

MustacheLite constexpr &operator<<(MustacheLite &ar, const ValueType &tag) {
  return ar;
}

MustacheLite constexpr &operator<<(MustacheLite &ar,
                                   const Collection<ValueType> &tag) {
  return ar;
}

MustacheLite constexpr &operator<<(MustacheLite &ar,
                                   const Map<String, ValueType> &tag) {
  return ar;
}

} // namespace Modules::Templating