#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/SQL/Base/QueryBuilder.hpp"
#include "Modules/Serialization/Base/ArrayTag.hpp"
#include "Modules/Serialization/Base/KeyValueTag.hpp"
#include "Modules/Serialization/Base/ObjectTag.hpp"
#include <Core/Logging/LoggerManager.hpp>

#include <Modules/SQL/Base/Testing/BasicUseTestCase.hpp>

#include <Modules/Serialization/Base/AbstractArchiver.hpp>
#include <cassert>

#include <sqlite3.h>
#include <string>

using namespace Modules::Serialization::Base;
using namespace Modules::SQL::Base;

using QueryResult = Collection<SharedPointer<Map<String, String>>>;

class SQLiteDriver : public Creator<SQLiteDriver>,
                     public EnableSharedFromThis<SQLiteDriver> {
public:
  SQLiteDriver(const String &filename) : filename(filename) {

    int retval = sqlite3_open(filename.c_str(), &db);

    if (retval != SQLITE_OK) {
      throw Core::Exceptions::RuntimeException(
          "Failed to open SQLite3 databe in {}", filename);
    }
  }

  virtual ~SQLiteDriver() { sqlite3_close(db); }

  using SQLiteCallbackType = std::function<void(void *handle_type, int argc,
                                                char **value, char **key)>;

  QueryResult query(SharedPointer<Modules::SQL::Base::QueryBuilder> query) {
    QueryResult collection;

    struct Callback {
      QueryResult *coll;
      static int callback(void *priv, int argc, char **values, char **names) {
        auto el = SharedPointer<Map<String, String>>::make();
        auto *cb = static_cast<Callback *>(priv);

        for (int i = 0; i < argc; i++) {
          (*el)[names[i]] = values[i];
        }

        cb->coll->push_back(el);
        return 0;
      }
    };

    Callback cb{&collection};
    char *err_msg = nullptr;

    sqlite3_exec(db, query->compile().c_str(), Callback::callback, &cb,
                 &err_msg);

    if (err_msg != nullptr) {
      throw Core::Exceptions::RuntimeException("Failed to execute query {}",
                                               err_msg);
    }

    return collection;
  }

  String tags() { return String::join(restrictions, " "); }

  void add_tag(const ::QueryBuilder::Tag &tag) {
    restrictions.push_back(tag.value());
  }

private:
  String filename;
  sqlite3 *db = nullptr;
  Collection<String> restrictions;
};

template <typename ValueType> ValueType from_string(const String &value);
template <> String from_string(const String &value) { return value; }
template <> int from_string(const String &value) {
  return std::stoi(value.c_str());
}
template <> double from_string(const String &value) {
  return std::stod(value.c_str());
}

class SQLiteInputArchiver : public SQLiteDriver {
public:
  using SQLiteDriver::SQLiteDriver;
  SharedPointer<QueryBuilder> expression;
  Collection<std::function<void()>> callbacks;
  SharedPointer<Map<String, String>> result;

private:
};

template <typename Archiver>
Archiver constexpr &operator%(Archiver &ar,
                              const SharedPointer<ArrayTag> &tag) {
  return ar;
}

template <typename Archiver>
Archiver constexpr &operator%(Archiver &ar,
                              const SharedPointer<ObjectTag> &tag) {

  if (tag->part == TagPart::Start) {
    ar.expression = QueryBuilder::create()
                        ->select("*")
                        ->from("{}", tag->name.c_str())
                        ->limit("1");
    ar.result = ar.query(ar.expression).front();
  }

  if (tag->part == TagPart::End) {
    for (auto &fn : ar.callbacks) {
      fn();
    }
  }

  return ar;
}

template <typename Archiver>
Archiver constexpr &operator%(Archiver &ar,
                              SharedPointer<KeyValueTag<double>> tag) {
  ar.callbacks.push_back(
      [&ar, tag]() { tag->value = (*ar.result)[tag->name]; });
  return ar;
}

template <typename Archiver>
Archiver constexpr &operator%(Archiver &ar,
                              SharedPointer<KeyValueTag<int>> tag) {
  ar.callbacks.push_back([&ar, tag]() {
    int value = from_string<int>((*ar.result)[tag->name]);
    *(tag->value) = value;
  });
  return ar;
}

template <typename Archiver>
Archiver constexpr &operator%(Archiver &ar,
                              SharedPointer<KeyValueTag<String>> tag) {
  ar.callbacks.push_back([&ar, tag]() {
    String value = (*ar.result)[tag->name];
    *(tag->value) = value;
  });
  return ar;
}

struct Person {
  int id;
  String name;
  int age;
};

template <typename Archiver> Archiver &operator%(Archiver &ar, Person &person) {
  ar % ArchiveTagFactory::make_object_start("Person");
  ar % ArchiveTagFactory::make_named_value_property("id", person.id);
  ar % ArchiveTagFactory::make_named_value_property("name", person.name);
  ar % ArchiveTagFactory::make_named_value_property("age", person.age);
  ar % ArchiveTagFactory::make_object_end("Person");
  return ar;
}

SharedPointer<SQLiteDriver> operator>>(SharedPointer<SQLiteDriver> driver,
                                       QueryResult &collection) {
  auto query_string =
      Modules::SQL::Base::QueryBuilder::create()->append_tag(driver->tags());
  collection.append_range(driver->query(query_string));
  return driver;
}

SharedPointer<SQLiteDriver>
operator>>(SharedPointer<SQLiteDriver> driver,
           const Modules::SQL::Base::QueryBuilder::Tag &tag) {
  driver->add_tag(tag);
  return driver;
}

int main(int argc, char *argv[]) {

  Core::Logging::LoggerManager::level_set(
      Core::Logging::LoggerManager::Level::Debug);
  Core::Logging::LoggerManager::stream_set(
      Core::Logging::LoggerManager::stream_cout());

  auto archiver = SQLiteInputArchiver("filename.db");
  auto person = Person();
  archiver % person;

  Core::Logging::LoggerManager::info("{} {} {}", person.id, person.name.c_str(),
                                     person.age);

  return 0;
}