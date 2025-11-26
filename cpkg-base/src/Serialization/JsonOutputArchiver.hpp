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
  JsonOutputArchiver(const std::string &path) : _stream(path) {}

  std::ostream &stream() { return _stream; }
  std::osyncstream syncstream() { return std::osyncstream(_stream); }

private:
  std::ofstream _stream;
};

inline JsonOutputArchiver &operator&(JsonOutputArchiver &ar, const int &value) {
  ar.syncstream() << value;
  return ar;
}

inline JsonOutputArchiver &operator&(JsonOutputArchiver &ar,
                                     const ValueDescriptor<int> &value) {
  ar.syncstream() << std::quoted(value.name) << ":" << value.value;
  return ar;
}

inline JsonOutputArchiver &operator&(JsonOutputArchiver &ar,
                                     const double &value) {
  ar.syncstream() << value;
  return ar;
}

inline JsonOutputArchiver &operator&(JsonOutputArchiver &ar,
                                     const ValueDescriptor<double> &value) {
  ar.syncstream() << std::quoted(value.name) << ":" << value.value;
  return ar;
}

inline JsonOutputArchiver &operator&(JsonOutputArchiver &ar,
                                     const bool &value) {
  ar.syncstream() << ((value) ? "true" : "false");
  return ar;
}

inline JsonOutputArchiver &operator&(JsonOutputArchiver &ar,
                                     const ValueDescriptor<bool> &value) {
  ar.syncstream() << std::quoted(value.name) << ":"
                  << ((value.value) ? "true" : "false");
  return ar;
}

inline JsonOutputArchiver &operator&(JsonOutputArchiver &ar,
                                     const std::string &value) {
  ar.syncstream() << std::quoted(value);
  return ar;
}

inline JsonOutputArchiver &
operator&(JsonOutputArchiver &ar, const ValueDescriptor<std::string> &value) {
  ar.syncstream() << std::quoted(value.name) << ":" << std::quoted(value.value);
  return ar;
}

template <typename ContainedType>
inline JsonOutputArchiver &operator&(JsonOutputArchiver &ar,
                                     const std::deque<ContainedType> &value) {
  ar.syncstream() << std::quoted(value);
  return ar;
}

template <typename ContainedType>
inline JsonOutputArchiver &
operator&(JsonOutputArchiver &ar,
          const ValueDescriptor<std::deque<ContainedType>> &descriptor) {

  auto [name, value] = descriptor;

  ar.syncstream() << std::quoted(name) << ":";

  ar.syncstream() << "[";

  if (value.size() == 0) {
    return ar;
  }

  ar & value.front();
  value.pop_front();

  for (auto el : value) {
    ar.syncstream() << ",";
    ar & el;
  }

  ar.syncstream() << "]";

  return ar;
}

} // namespace Serialization