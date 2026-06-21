#pragma once

#include "CXPM/Modules/Serialization/AbstractOutputArchiver.hpp"
#include "CXPM/Modules/Serialization/ArrayEndToken.hpp"
#include "CXPM/Modules/Serialization/ArrayStartToken.hpp"
#include "CXPM/Modules/Serialization/ObjectEndToken.hpp"
#include "CXPM/Modules/Serialization/ObjectStartToken.hpp"
#include "CXPM/Modules/Serialization/PairToken.hpp"
#include "CXPM/Modules/Serialization/SeparatorToken.hpp"
#include "CXPM/Modules/Serialization/ValueToken.hpp"
#include <CXPM/Core/UniquePointer.hpp>
#include <CXPM/Utils/Unused.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ostream>

namespace CXPM::Modules::Serialization {

struct JsonOutputArchiver : public virtual AbstractOutputArchiver {
  explicit JsonOutputArchiver(std::ostream &ostream)
      : AbstractOutputArchiver(ostream) {}
};

static inline JsonOutputArchiver &operator%(JsonOutputArchiver &joa,
                                            const ArrayStartToken &) {
  joa.stream_get() << "[";
  return joa;
};

static inline JsonOutputArchiver &operator%(JsonOutputArchiver &joa,
                                            const ArrayEndToken &) {
  joa.stream_get() << "]";
  return joa;
};

static inline JsonOutputArchiver &operator%(JsonOutputArchiver &joa,
                                            const ObjectStartToken &) {
  joa.stream_get() << "{";
  return joa;
};

static inline JsonOutputArchiver &operator%(JsonOutputArchiver &joa,
                                            const ObjectEndToken &) {
  joa.stream_get() << "}";
  return joa;
};

static inline JsonOutputArchiver &operator%(JsonOutputArchiver &joa,
                                            const SeparatorToken &) {
  joa.stream_get() << ",";
  return joa;
};

static inline JsonOutputArchiver &
operator%(JsonOutputArchiver &joa, const ValueToken<bool> &token) {
  joa.stream_get() << (token.value_get() ? "true": "false");
  return joa;
};

static inline JsonOutputArchiver &
operator%(JsonOutputArchiver &joa, const ValueToken<std::nullptr_t> &) {
  joa.stream_get() << "null";
  return joa;
};

static inline JsonOutputArchiver &
operator%(JsonOutputArchiver &joa, const ValueToken<std::int32_t> & token) {
  joa.stream_get() << std::to_string(token.value_get());
  return joa;
};

static inline JsonOutputArchiver &
operator%(JsonOutputArchiver &joa, const ValueToken<std::double_t> & token) {
  joa.stream_get() << std::to_string(token.value_get());
  return joa;
};

static inline JsonOutputArchiver &
operator%(JsonOutputArchiver &joa, const ValueToken<const String> & token) {
  joa.stream_get() << std::quoted(token.value_get().c_str());
  return joa;
};

template<std::size_t size>
static inline JsonOutputArchiver &
operator%(JsonOutputArchiver &joa, const ValueToken<const char[size]> & token) {
  joa.stream_get() << std::quoted(token.value_get());
  return joa;
};

template <typename KeyType, typename ValueType>
static inline JsonOutputArchiver &
operator%(JsonOutputArchiver &joa, const PairToken<KeyType, ValueType> &value) {
  
  joa % ValueToken{value.key_get()};
  joa.stream_get() << ":";
  joa % ValueToken{value.value_get()};

  return joa;
};

} // namespace CXPM::Modules::Serialization