#pragma once

#include "Modules/SQL/Base/QueryBuilder.hpp"
#include "Modules/SQL/SQLite/SQLiteDriver.hpp"
#include "Modules/Serialization/Base/KeyValueTag.hpp"
#include "Modules/Serialization/Base/ObjectTag.hpp"

using namespace Modules::Serialization::Base;
using namespace Modules::SQL::Base;

namespace Modules::SQL::SQLite {
class SQLiteOutputArchiver : public SQLiteDriver {
public:
  using SQLiteDriver::SQLiteDriver;
  SharedPointer<QueryBuilder> expression;
  Collection<std::function<void()>> callbacks;
  SharedPointer<Map<String, String>> result;

private:
};

SQLiteOutputArchiver constexpr &operator%(SQLiteOutputArchiver &ar,
                                          const SharedPointer<ObjectTag> &tag) {

  if (tag->part == TagPart::Start) {
    ar.expression = QueryBuilder::create()
                        ->insert()
                        ->into("{}", tag->name.c_str())
                        ->values_start();
  }

  if (tag->part == TagPart::End) {
    ar.expression->values_end();
    ar.query(ar.expression);
  }

  return ar;
}

SQLiteOutputArchiver constexpr &
operator%(SQLiteOutputArchiver &ar, SharedPointer<KeyValueTag<double>> tag) {
  ar.expression->value(*tag->value);
  return ar;
}

SQLiteOutputArchiver constexpr &operator%(SQLiteOutputArchiver &ar,
                                          SharedPointer<KeyValueTag<int>> tag) {
  ar.expression->value(*tag->value);
  return ar;
}

SQLiteOutputArchiver constexpr &
operator%(SQLiteOutputArchiver &ar, SharedPointer<KeyValueTag<String>> tag) {
  ar.expression->value(*tag->value);
  return ar;
}
} // namespace Modules::SQL::SQLite