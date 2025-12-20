#pragma once

#include "Core/Logging/Manager.hpp"
#include "Modules/SQL/Base/Driver.hpp"
#include "Modules/SQL/Base/QueryBase.hpp"
#include "Modules/SQL/Base/QueryBuilder.hpp"
#include "Modules/SQL/Base/QueryResult.hpp"
#include <Core/Containers/String.hpp>
#include <Core/Exceptions/RuntimeException.hpp>
#include <Utils/Patterns/Creator.hpp>

#include <sqlite3.h>

using namespace Utils::Patterns;

using namespace Core::Containers;
using namespace Modules::SQL::Base;

namespace Modules::SQL::SQLite {
class SQLiteDriver : public Creator<SQLiteDriver>,
                     public EnableSharedFromThis<SQLiteDriver>, public Driver {
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

  QueryResult query(SharedPointer<QueryBase> query) override {
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

    Core::Logging::Logger::info("{}", query->compile().c_str());

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

inline SharedPointer<SQLiteDriver> operator>>(SharedPointer<SQLiteDriver> driver,
                                       QueryResult &collection) {
  auto query_string =
      Modules::SQL::Base::QueryBuilder::create()->append_tag(driver->tags());
std::copy(driver->query(query_string).begin(), driver->query(query_string).end(), std::back_inserter(  collection));
  return driver;
}

SharedPointer<SQLiteDriver>
inline operator>>(SharedPointer<SQLiteDriver> driver,
           const Modules::SQL::Base::QueryBuilder::Tag &tag) {
  driver->add_tag(tag);
  return driver;
}

} // namespace Modules::SQL::SQLite