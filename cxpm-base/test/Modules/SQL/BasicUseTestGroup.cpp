#include <Core/Logging/LoggerManager.hpp>

#include <Modules/SQL/Base/Testing/BasicUseTestCase.hpp>

#include <Modules/Serialization/Base/AbstractArchiver.hpp>
#include <cassert>

#include <Modules/SQL/SQLite/SQLiteInputArchiver.hpp>
#include <Modules/SQL/SQLite/SQLiteOutputArchiver.hpp>

#include <sqlite3.h>
#include <string>

using namespace Modules::Serialization::Base;
using namespace Modules::SQL::SQLite;

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

int main(int argc, char *argv[]) {

  Core::Logging::LoggerManager::level_set(
      Core::Logging::LoggerManager::Level::Debug);
  Core::Logging::LoggerManager::stream_set(
      Core::Logging::LoggerManager::stream_cout());

  {
    auto output = SQLiteOutputArchiver("filename.db");
    auto person = Person();
    person.name = "Arthur";
    person.age = 36;
    output % person;
  }

  {
    auto input = SQLiteInputArchiver("filename.db");
    auto person = Person();
    input % person;
    Core::Logging::LoggerManager::info("{} {} {}", person.id,
                                       person.name.c_str(), person.age);
  }

  return 0;
}