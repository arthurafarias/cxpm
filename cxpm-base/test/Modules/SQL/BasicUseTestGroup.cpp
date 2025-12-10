#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/SQL/Base/QueryBuilder.hpp"
#include <Core/Logging/LoggerManager.hpp>

#include <Modules/SQL/Base/Testing/BasicUseTestCase.hpp>

#include <Modules/Serialization/Base/AbstractArchiver.hpp>
#include <cassert>

#include <sqlite3.h>

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

const char sql[] = R"(
CREATE TABLE "Person" (
	"id"	INTEGER,
	"name"	TEXT,
	PRIMARY KEY("id" AUTOINCREMENT)
);
)";

SharedPointer<SQLiteDriver> operator>>(SharedPointer<SQLiteDriver> driver,
                                       QueryResult &collection) {
  auto query_string = Modules::SQL::Base::QueryBuilder::create()
                          ->append_tag(driver->tags());
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

  try {
    auto driver = SQLiteDriver::create("filename.db");

    auto results = QueryResult();

    // driver >> QueryBuilder::Limit(1) >> results;
    driver >> QueryBuilder::Tag("SELECT *")
           >> QueryBuilder::Tag("FROM Person")
           >> QueryBuilder::Tag("ORDER BY id")
           >> QueryBuilder::Tag("LIMIT 1")
           >> results;

    for (auto result : results) {
      std::cout << "Name: " << (*result)["name"] << std::endl;
    }
  } catch (Core::Exceptions::RuntimeException &ex) {
    Core::Logging::LoggerManager::error("Failed: {}", ex.what());
  }

  return 0;
}