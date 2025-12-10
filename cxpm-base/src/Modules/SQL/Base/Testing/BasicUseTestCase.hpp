// #pragma once

// #include "Core/Containers/Collection.hpp"
// #include "Core/SharedPointer.hpp"
// #include "Modules/SQL/Base/QueryBuilder.hpp"
// #include "Modules/Testing/TestCase.hpp"
// #include <initializer_list>

// namespace Modules::SQL::Base::Testing {

// class FieldBase {};

// class FieldToken : public FieldBase {
//   String name;
//   String value;
// };

// class OperatorToken : public FieldBase {
//   String name;
//   String value;
// };

// class ValueToken : public FieldBase {
//   String value;
// };

// class Expression : Collection<FieldBase> {
// public:
// };

// struct PersonModel {
//   String name;
//   int age;
// };

// class SQLiteDriver : public DriverInterface, public Creator<SQLiteDriver> {
// public:
//   SQLiteDriver(const String &file) {}

//   virtual void commit() override {
//     // commited!
//   }

// };

// class BasicUseTestCase : public Modules::Testing::TestCase {
// public:
//   BasicUseTestCase()
//       : Modules::Testing::TestCase("Modules::SQL:Base::BasicUse", "Basic Use") {
//   }

//   virtual void run() override {

//     SharedPointer<DriverInterface> driver = SQLiteDriver::create("file.db");

//     for (auto person : QueryBuilder<PersonModel>::create(driver)
//                            ->select("*")
//                            ->where("name == `{}`", "arthur")
//                            ->execute()) {
//       person->age = person->age + 1;
//       person->save();
//     }

//     driver->commit();
//   }
// };
// } // namespace Modules::SQL::Base::Testing