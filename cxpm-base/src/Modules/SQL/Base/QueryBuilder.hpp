// ---------------------------------------------------------------------------
// PROPRIETARY CODE – Arthur de Araújo Farias 2025
// All rights reserved.  No part of this file may be reproduced, stored in a
// retrieval system, or transmitted in any form or by any means—electronic,
// mechanical, photocopying, recording, or otherwise—without the prior written
// permission of the copyright holder.
// ---------------------------------------------------------------------------

#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Object.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/SQL/Base/Driver.hpp"
#include "Modules/SQL/Base/QueryBase.hpp"
#include "Utils/Patterns/Creator.hpp"

#include <format>

using namespace Core::Containers;
using namespace Utils::Patterns;

// I will handle this later gonna sleep now and see if tommorow I can create
// some stuff really cool to in this direction!

namespace Modules::SQL::Base {

class QueryBuilder : public EnableSharedFromThis<QueryBuilder>,
                     public Utils::Patterns::Creator<QueryBuilder>,
                     public Core::Object,
                     public QueryBase {
public:
  enum class FieldTpe { TEXT, INTEGER, REAL, BLOB, NUMERIC };

  class Field {
  public:
    FieldTpe type;
    String name;
    bool unique;
    bool not_null;
    bool primary_key;
  };

  class Tag {
  public:
    Tag(const String &tag) : tag(tag) {}
    String value() const { return tag; }

  private:
    String tag;
  };

  class Limit : public Tag {
  public:
    Limit(int value = -1)
        : Tag(std::format("LIMIT {}", std::to_string(value))) {}
  };

  QueryBuilder() {}
  QueryBuilder(QueryBuilder &) {}
  QueryBuilder &operator=(QueryBuilder &) { return *this; }

  QueryBuilder(QueryBuilder &&) {}
  QueryBuilder &operator=(QueryBuilder &&) { return *this; }

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder>
  create_table(const std::format_string<FormatTypes...> &fmt,
               FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    query.push_back(std::format("CREATE TABLE {}", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder>
  insert_into(const std::format_string<FormatTypes...> &fmt,
              FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    query.push_back(std::format("INSERT INTO {}", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename... FormatTypes> SharedPointer<QueryBuilder> insert() {
    auto lock = query.acquire_lock();
    query.push_back(std::format("INSERT"));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder>
  field(const std::format_string<FormatTypes...> &fmt, FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    fields.push_back(std::format("'{}'", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  };

  SharedPointer<QueryBuilder> fields_start() {
    auto lock = query.acquire_lock();
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  };

  SharedPointer<QueryBuilder> fields_end() {
    auto lock = query.acquire_lock();
    query.push_back(std::format("( {} )", String::join(fields, ",").c_str()));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  };

  SharedPointer<QueryBuilder> values_start() {
    auto lock = query.acquire_lock();
    query.push_back(std::format("VALUES "));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  };

  SharedPointer<QueryBuilder> values_end() {
    auto lock = query.acquire_lock();
    query.push_back(std::format("( {} )", String::join(values, ",").c_str()));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  };

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder>
  into(const std::format_string<FormatTypes...> &fmt, FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    query.push_back(std::format("INTO {}", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  SharedPointer<QueryBuilder> value(const String &value) {
    auto lock = query.acquire_lock();
    values.push_back(std::format("'{}'", value.c_str()));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  SharedPointer<QueryBuilder> value(const double &value) {
    auto lock = query.acquire_lock();
    values.push_back(std::format("{}", value));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  SharedPointer<QueryBuilder> value(const int &value) {
    auto lock = query.acquire_lock();
    values.push_back(std::format("{}", value));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder>
  select(const std::format_string<FormatTypes...> &fmt, FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    query.push_back(std::format("SELECT {}", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder>
  where(const std::format_string<FormatTypes...> &fmt, FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    query.push_back(std::format("WHERE {}", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder>
  from(const std::format_string<FormatTypes...> &fmt, FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    query.push_back(std::format("FROM {}", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder> order_by(std::format_string<FormatTypes...> fmt,
                                       const FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    query.push_back(std::format("ORDER BY {}", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  SharedPointer<QueryBuilder> desc() {
    auto lock = query.acquire_lock();
    query.push_back(std::format("DESC"));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  SharedPointer<QueryBuilder> asc() {
    auto lock = query.acquire_lock();
    query.push_back(std::format("ASC"));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename... FormatTypes>
  SharedPointer<QueryBuilder>
  limit(const std::format_string<FormatTypes...> &fmt, FormatTypes &&...args) {
    auto lock = query.acquire_lock();
    auto compiled = std::format(fmt, std::forward<FormatTypes>(args)...);
    query.push_back(std::format("LIMIT {}", compiled));
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  SharedPointer<QueryBuilder> append_tag(const String &tag) {
    query.push_back(tag);
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  String compile() {
    auto lock = query.acquire_lock();
    return String::join(query, " ") + ";";
  }

  SharedPointer<QueryBuilder> exec(SharedPointer<Driver> driver) {
    result = driver->query(shared_from_this());
    return EnableSharedFromThis<QueryBuilder>::shared_from_this();
  }

  template <typename TargetType> Collection<SharedPointer<TargetType>> cast() {
    Collection<SharedPointer<TargetType>> casted;
    result >> casted;
    return casted;
  }

private:
  Collection<SharedPointer<Map<String, String>>> result;
  Collection<String> query;

  Collection<String> fields;
  Collection<String> values;
};

} // namespace Modules::SQL::Base