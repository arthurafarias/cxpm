#pragma once

#include "Core/Containers/Collection.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Object.hpp"
#include "Core/SharedPointer.hpp"
#include "Utils/Patterns/Creator.hpp"

#include <format>

using namespace Core::Containers;
using namespace Utils::Patterns;

// I will handle this later gonna sleep now and see if tommorow I can create
// some stuff really cool to in this direction!

namespace Modules::SQL::Base {

class QueryBuilder : public EnableSharedFromThis<QueryBuilder>,
                     public Utils::Patterns::Creator<QueryBuilder>,
                     public Core::Object {
public:
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
    query.push_back(std::format("SELECT {}", compiled));
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

private:
  Collection<String> query;
};

} // namespace Modules::SQL::Base