#pragma once

#include <CXPM/Core/Exceptions/NotImplementedException.hpp>
#include <CXPM/Core/Exceptions/RuntimeException.hpp>
#include <CXPM/Core/SharedPointer.hpp>
#include <CXPM/Modules/Serialization/AbstractArchiver.hpp>
#include <CXPM/Modules/Serialization/ValueDescriptor.hpp>
#include <CXPM/Utils/Unused.hpp>
#include <CXPM/Core/UniquePointer.hpp>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <memory>
#include <ostream>
#include <syncstream>

namespace CXPM::Modules::Serialization {

class JsonOutputArchiver : public AbstractArchiver {
public:
  explicit JsonOutputArchiver(std::ostream &ostream)
      : stream_property(ostream) {}

  std::osyncstream stream() { return std::osyncstream(stream_property); }

  virtual String to_string() { return ""; }

  void object_start(TagBase tag) {
    stream() << "{";
    multipart_element_stack.push_front(tag);
  }

  void object_end(TagBase tag) {
    validate_tag(tag);
    stream() << "}";
    multipart_element_stack.pop_front();
  }

  void array_start(TagBase tag) {
    stream() << "[";
    multipart_element_stack.push_front(tag);
  }

  void array_end(TagBase tag) {
    validate_tag(tag);
    stream() << "]";
    multipart_element_stack.pop_front();
  }

  template <typename ValueType> void key_value(KeyValueTag<ValueType> tag) {
    if (multipart_element_stack.empty()) {
      throw Core::Exceptions::RuntimeException(
          "Can't place a key-value node outside an object!");
    }

    try_print_comma();
    stream() << std::quoted(tag.name) << ":";
    (*this) % ValueTag{tag.value};
  }

  void multipart(const TagBase &tag) {
    if (tag.part == TagPart::Start) {
      multipart_element_stack.push_front(tag);
    } else {
      if (tag.name != multipart_element_stack.front().name) {
        throw Core::Exceptions::RuntimeException(
            "Closing element with different name.");
      }
      multipart_element_stack.pop_front();
    }
  }

  template <typename ValueType> void value(const ValueTag<ValueType> &tag) {
    static int unique_id = 0;
    unique_id++;

    object_start(TagBase{"object-" + std::to_string(unique_id), TagPart::Start,
                         TagType::Object});
    (*this) % tag.value;
    object_end(TagBase{"object-" + std::to_string(unique_id), TagPart::End,
                       TagType::Object});
  }

  void try_print_comma() {
    if (!multipart_element_stack.empty() &&
        (multipart_element_stack.front().part == TagPart::Start)) {
      multipart_element_stack.front().part = TagPart::End;
    } else {
      stream() << ",";
    }
  }

private:
  void validate_tag(TagBase &tag) {
    if (multipart_element_stack.empty())
      return;

    auto current_tag = multipart_element_stack.front();
    if (current_tag.type != tag.type || current_tag.name != tag.name) {
      throw Core::Exceptions::RuntimeException(
          "Failed to serialize element at tag {}", tag.name);
    }
  }

private:
  Collection<TagBase> multipart_element_stack;
  std::ostream &stream_property;
};

template <>
inline void JsonOutputArchiver::value<int>(const ValueTag<int> &tag) {
  stream() << tag.value;
}

template <>
inline void JsonOutputArchiver::value<double>(const ValueTag<double> &tag) {
  stream() << tag.value;
}

template <>
inline void JsonOutputArchiver::value<String>(const ValueTag<String> &tag) {
  stream() << std::quoted(tag.value);
}

template <>
inline void JsonOutputArchiver::value<bool>(const ValueTag<bool> &tag) {
  stream() << tag.value;
}

template <>
inline void
JsonOutputArchiver::value<std::nullptr_t>(const ValueTag<std::nullptr_t> &tag) {
  Utils::Unused{tag};
  throw Core::Exceptions::NotImplementedException();
}

template <typename ValueType>
inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const ValueTag<ValueType> &value) {
  ar.value(value);
  return ar;
}

template <typename ValueType>
inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const KeyValueTag<ValueType> &value) {
  ar.key_value(value);
  return ar;
}

template <typename ValueType>
inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const TagBase &descriptor) {
  ar.multipart(descriptor);
  return ar;
}

template <typename ElementType>
inline JsonOutputArchiver &
operator%(JsonOutputArchiver &ar, const Collection<ElementType> &collection) {
  static int unique_id = 0;
  unique_id++;

  ar.array_start(TagBase("array-" + std::to_string(unique_id), TagPart::Start,
                         TagType::Array));

  for (const auto &el : collection) {
    ar.try_print_comma();
    ar.value(ValueTag<ElementType>(el));
  }

  ar.array_end(TagBase("array-" + std::to_string(unique_id), TagPart::End,
                       TagType::Array));
  return ar;
}

} // namespace CXPM::Modules::Serialization
