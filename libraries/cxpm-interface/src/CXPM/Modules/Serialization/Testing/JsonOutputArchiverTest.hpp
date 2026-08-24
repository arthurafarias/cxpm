#pragma once

#include "CXPM/Modules/Serialization/JsonOutputArchiver.hpp"
#include "CXPM/Testing/TestGroup.hpp"

#include <sstream>

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Modules::Serialization::Testing {

struct JsonOutputArchiverTest : public TestGroup {
  JsonOutputArchiverTest()
      : TestGroup(
            "JsonOutputArchiver",
            {
                {"bool tokens render as JSON true/false",
                 [](TestContext &ctx) {
                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   bool value = true;
                   archiver % CXPM::Modules::Serialization::ValueToken{value};
                   ctx.equal(stream.str(), std::string("true"));
                 }},
                {"nullptr_t tokens render as JSON null",
                 [](TestContext &ctx) {
                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   std::nullptr_t value = nullptr;
                   archiver % CXPM::Modules::Serialization::ValueToken{value};
                   ctx.equal(stream.str(), std::string("null"));
                 }},
                {"int32 tokens render as plain JSON numbers",
                 [](TestContext &ctx) {
                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   std::int32_t value = 42;
                   archiver % CXPM::Modules::Serialization::ValueToken{value};
                   ctx.equal(stream.str(), std::string("42"));
                 }},
                {"String tokens render as quoted JSON strings",
                 [](TestContext &ctx) {
                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   const String value = "hello";
                   archiver % CXPM::Modules::Serialization::ValueToken{value};
                   ctx.equal(stream.str(), std::string("\"hello\""));
                 }},
                {"a PairToken renders as a quoted JSON key/value pair",
                 [](TestContext &ctx) {
                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   const String key = "name";
                   const String value = "cxpm";
                   archiver % CXPM::Modules::Serialization::PairToken{key,
                                                                      value};
                   ctx.equal(stream.str(), std::string("\"name\":\"cxpm\""));
                 }},
                {"Object/Array start and end tokens render as JSON "
                 "structural characters",
                 [](TestContext &ctx) {
                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   archiver %
                       CXPM::Modules::Serialization::ObjectStartToken{"obj"};
                   archiver % CXPM::Modules::Serialization::ArrayStartToken{
                                  "arr"};
                   archiver % CXPM::Modules::Serialization::SeparatorToken{};
                   archiver %
                       CXPM::Modules::Serialization::ArrayEndToken{"arr"};
                   archiver %
                       CXPM::Modules::Serialization::ObjectEndToken{"obj"};
                   ctx.equal(stream.str(), std::string("{[,]}"));
                 }},
                {"an empty BasicCollection<String> serializes as an empty "
                 "JSON array instead of invoking undefined behavior",
                 [](TestContext &ctx) {
                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   BasicCollection<String> values;
                   archiver %
                       CXPM::Modules::Serialization::ValueToken{values};
                   ctx.equal(stream.str(), std::string("[]"));
                 }},
                {"a non-empty BasicCollection<String> serializes as a "
                 "comma-separated JSON array",
                 [](TestContext &ctx) {
                   std::ostringstream stream;
                   CXPM::Modules::Serialization::JsonOutputArchiver archiver(
                       stream);
                   BasicCollection<String> values{"a", "b"};
                   archiver %
                       CXPM::Modules::Serialization::ValueToken{values};
                   ctx.equal(stream.str(), std::string("[\"a\",\"b\"]"));
                 }},
            }) {}
};

inline static JsonOutputArchiverTest json_output_archiver_test_instance;

} // namespace CXPM::Modules::Serialization::Testing
