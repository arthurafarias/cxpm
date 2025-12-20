#pragma once

#include "Modules/SQL/SQLite/SQLiteDriver.hpp"
#include "Modules/Serialization/Base/KeyValueTag.hpp"
#include "Modules/Serialization/Base/ObjectTag.hpp"

using namespace Modules::Serialization::Base;
using namespace Modules::SQL::Base;

namespace Modules::SQL::SQLite {

namespace {
template <typename ValueType> ValueType from_string(const String &value);
template <> String from_string(const String &value) { return value; }
template <> int from_string(const String &value) {
  return std::stoi(value.c_str());
}
template <> double from_string(const String &value) {
  return std::stod(value.c_str());
}
} // namespace

class SQLiteInputArchiver : public SQLiteDriver {
public:
  using SQLiteDriver::SQLiteDriver;
  SharedPointer<QueryBuilder> expression;
  Collection<std::function<void()>> callbacks;
  SharedPointer<Map<String, String>> result;

private:
};

inline SQLiteInputArchiver &operator%(SQLiteInputArchiver &ar,
                                         const SharedPointer<ObjectTag> &tag) {

  if (tag->part == TagPart::Start) {
    ar.expression = QueryBuilder::create()
                        ->select("*")
                        ->from("{}", tag->name.c_str())
                        ->limit("1");
    ar.result = ar.query(ar.expression).front();
  }

  if (tag->part == TagPart::End) {
    for (auto fn : ar.callbacks) {
      fn();
    }
  }

  return ar;
}

inline SQLiteInputArchiver &
operator%(SQLiteInputArchiver &ar, SharedPointer<KeyValueTag<double>> tag) {
  ar.callbacks.push_back([&ar, tag]() {
    double value = from_string<double>((*ar.result)[tag->name]);
    (*tag->value) = value;
  });
  return ar;
}

inline SQLiteInputArchiver &operator%(SQLiteInputArchiver &ar,
                                         SharedPointer<KeyValueTag<int>> tag) {
  ar.callbacks.push_back([&ar, tag]() {
    int value = from_string<int>((*ar.result)[tag->name]);
    *(tag->value) = value;
  });
  return ar;
}

inline SQLiteInputArchiver &
operator%(SQLiteInputArchiver &ar, SharedPointer<KeyValueTag<String>> tag) {
  ar.callbacks.push_back([&ar, tag]() {
    String value = (*ar.result)[tag->name];
    *(tag->value) = value;
  });
  return ar;
}

} // namespace Modules::SQL::SQLite