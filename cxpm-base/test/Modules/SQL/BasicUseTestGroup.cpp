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

class Row : public Map<String, String>, public EnableSharedFromThis<Row> {
public:
  using Map<String, String>::Map;
};

class QueryResult : public Collection<Row>,
                    public EnableSharedFromThis<QueryResult> {
public:
  using Collection<Row>::Collection;
};

class PersonModel {
public:
  String name;

  PersonModel(int argc, char **names, char **values) {
    for (int i = 0; i < argc; i++) {
      if (String(names[i]) == "name") {
        name = String(values[i]);
      }
    }
  }
  template <typename Archiver>
  SharedPointer<Archiver> load(SharedPointer<Archiver> ar) {

    using namespace Modules::Serialization::Base;
    (*ar) % ArchiveTagFactory::make_object_start("Person");
    (*ar) % ArchiveTagFactory::make_named_value_property("name", name);
    (*ar) % ArchiveTagFactory::make_object_end("Person");
    return ar;
  }
};

class SQLTag {
public:
  SQLTag(const String &tag) : tag(tag) {}
  String value() const { return tag; }

private:
  String tag;
};

class LimitTag : public SQLTag {
public:
  LimitTag(int value = -1)
      : SQLTag(std::format("LIMIT {}", std::to_string(value))) {}
};

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

  template <typename ModelType>
  Collection<SharedPointer<ModelType>> query(
      const SharedPointer<Modules::SQL::Base::QueryBuilder<ModelType>> &query) {

    Collection<SharedPointer<ModelType>> collection;

    struct Callback {
      Collection<SharedPointer<ModelType>> *coll;
      static int callback(void *handle, int argc, char **values, char **names) {
        auto *cb = static_cast<Callback *>(handle);
        cb->coll->push_back(
            SharedPointer<ModelType>::make(argc, names, values));
        return 0;
      }
    };

    Callback cb{&collection};
    char *err_msg = nullptr;

    sqlite3_exec(db, query->compile().c_str(), Callback::callback, &cb,
                 &err_msg);

    return collection;
  }

  String tags() { return String::join(restrictions, " "); }

  void add_tag(const SQLTag &tag) { restrictions.push_back(tag.value()); }

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

SharedPointer<SQLiteDriver>
operator>>(SharedPointer<SQLiteDriver> driver,
           Collection<SharedPointer<PersonModel>> &collection) {
  auto query_string = Modules::SQL::Base::QueryBuilder<PersonModel>::create()
                          ->select("*")
                          ->from("Person")
                          ->order_by("id")
                          ->append_tag(driver->tags());

  collection.append_range(driver->query(query_string));
  return driver;
}

SharedPointer<SQLiteDriver> operator>>(SharedPointer<SQLiteDriver> driver,
                                       const SQLTag &tag) {
  driver->add_tag(tag);
  return driver;
}

int main(int argc, char *argv[]) {

  auto driver = SQLiteDriver::create("filename.db");

  auto results = Collection<SharedPointer<PersonModel>>();

  driver >> LimitTag(1) >> results;

  for (auto result : results) {
    std::cout << "Name: " << result->name << std::endl;
  }

  return 0;
}