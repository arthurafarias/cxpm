#include "Core/SharedPointer.hpp"
#include "Modules/SQL/Base/QueryBuilder.hpp"
#include "Modules/SQL/SQLite/SQLiteDriver.hpp"
#include <Core/Containers/Collection.hpp>
#include <Core/Logging/Manager.hpp>

#include <Modules/SQL/Base/Testing/BasicUseTestCase.hpp>

#include <Modules/Serialization/Base/AbstractArchiver.hpp>
#include <cassert>

#include <Modules/SQL/SQLite/SQLiteInputArchiver.hpp>
#include <Modules/SQL/SQLite/SQLiteOutputArchiver.hpp>

#include <sqlite3.h>
#include <string>

using namespace Core::Containers;
using namespace Modules::Serialization::Base;
using namespace Modules::SQL::SQLite;

struct Person {
  int id;
  String name;
  int age;
};

template <typename Archiver> Archiver &operator%(Archiver &ar, Person &person) {
  ar % TagFactory::make_object_start("Person");
  ar % TagFactory::make_named_value_property("id", person.id);
  ar % TagFactory::make_named_value_property("name", person.name);
  ar % TagFactory::make_named_value_property("age", person.age);
  ar % TagFactory::make_object_end("Person");
  return ar;
}

const Map<String, String> &operator>>(Map<String, String> &from, Person &to) {
  to.age = std::stoi(from["age"].c_str());
  to.name = from["name"];
  to.id = std::stoi(from["id"].c_str());
  return from;
}

const Map<String, String> &operator<<(Map<String, String> &to, Person &from) {
  to["name"] = from.name;
  to["id"] = std::to_string(from.id);
  to["age"] = std::to_string(from.age);
  return to;
}

const Collection<SharedPointer<Map<String, String>>> &
operator>>(const Collection<SharedPointer<Map<String, String>>> &from,
           Collection<SharedPointer<Person>> &to) {

  for (auto map : from) {
    auto person = SharedPointer<Person>::make();
    *map >> *person;
    to.push_back(person);
  }
  return from;
}

Collection<SharedPointer<Map<String, String>>> &
operator<<(Collection<SharedPointer<Map<String, String>>> &to,
           const Collection<SharedPointer<Person>> &from) {

  for (auto person : from) {
    auto map = SharedPointer<Map<String, String>>::make();
    *map << *person;
    to.push_back(map);
  }
  return to;
}

int main(int argc, char *argv[]) {

  Core::Logging::Logger::level_set(
      Core::Logging::Logger::Level::Debug);
  Core::Logging::Logger::stream_set(
      Core::Logging::Logger::stream_cout());

  {
    auto output = SQLiteOutputArchiver("database.db");
    auto person = Person();
    person.name = "Arthur";
    person.age = 36;
    output % person;
  }

  {
    auto input = SQLiteInputArchiver("database.db");
    auto person = Person();
    input % person;
    Core::Logging::Logger::info("{} {} {}", person.id,
                                       person.name.c_str(), person.age);
  }

  // Also is posible defining the >> and << operators
  {
    auto database = SQLiteDriver::create("database.db");

    for (auto person : QueryBuilder::create()
                           ->select("*")
                           ->from("Person")
                           ->order_by("id")
                           ->desc()
                           ->exec(database)->cast<Person>()) {

      Core::Logging::Logger::info("Person ID: {}, Name: {}, Age: {}",
                                         person->id, person->name.c_str(), person->age);
    }
  }

  return 0;
}