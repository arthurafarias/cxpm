#pragma once

#include <deque>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <string>
#include <syncstream>

#include <Serialization/AbstractArchiver.hpp>

namespace Serialization {

class JsonOutputArchiver : public AbstractArchiver {
public:
  JsonOutputArchiver(const String &path) : _stream(path) {}

  std::ostream &stream() { return _stream; }
  std::osyncstream syncstream() { return std::osyncstream(_stream); }

  virtual String to_string() override { return ""; }

private:
  std::ofstream _stream;
};

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar, const int &value) {
  ar.syncstream() << value;
  return ar;
}

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const ValueDescriptor<int> &value) {
  ar.syncstream() << std::quoted(value.name) << ":" << value.value;
  return ar;
}

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const double &value) {
  ar.syncstream() << value;
  return ar;
}

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const ValueDescriptor<double> &value) {
  ar.syncstream() << std::quoted(value.name) << ":" << value.value;
  return ar;
}

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const bool &value) {
  ar.syncstream() << ((value) ? "true" : "false");
  return ar;
}

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const ValueDescriptor<bool> &value) {
  ar.syncstream() << std::quoted(value.name) << ":"
                  << ((value.value) ? "true" : "false");
  return ar;
}

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const String &value) {
  ar.syncstream() << std::quoted(value);
  return ar;
}

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const ValueDescriptor<String> &value) {
  ar.syncstream() << std::quoted(value.name) << ":" << std::quoted(value.value);
  return ar;
}

template <typename ContainedType>
inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const Collection<ContainedType> &value) {

  ar.syncstream() << "[";

  if (value.size() > 0) {

    ar % value[0];

    for (auto i = 1; i < value.size(); i++) {
      ar.syncstream() << ",";
      ar % value[i];
    }
  }

  ar.syncstream() << "]";

  return ar;
}

template <typename ContainedType>
inline JsonOutputArchiver &
operator%(JsonOutputArchiver &ar,
          const ValueDescriptor<Collection<ContainedType>> &descriptor) {

  auto [name, value] = descriptor;

  ar.syncstream() << std::quoted(name) << ":";

  ar.syncstream() << "[";

  if (value.size() > 0) {

    ar.syncstream() % value[0];

    for (auto i = 1; i < value.size(); i++) {
      ar.syncstream() << ",";
      ar % value[i];
    }
  }

  ar.syncstream() << "]";

  return ar;
}

inline JsonOutputArchiver &operator%(JsonOutputArchiver &ar,
                                     const ElementDescriptor &descriptor) {
  switch (descriptor) {
  case Serialization::ElementDescriptor::ArrayStart:
    ar.syncstream() << "[";
    break;
  case Serialization::ElementDescriptor::ArrayEnd:
    ar.syncstream() << "]";
    break;
  case Serialization::ElementDescriptor::ObjectStart:
    ar.syncstream() << "{";
    break;
  case Serialization::ElementDescriptor::ObjectEnd:
    ar.syncstream() << "}";
    break;
  }

  return ar;
}

} // namespace Serialization