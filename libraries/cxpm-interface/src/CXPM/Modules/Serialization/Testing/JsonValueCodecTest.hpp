#pragma once

#include "CXPM/Modules/Serialization/JsonValueParser.hpp"
#include "CXPM/Modules/Serialization/JsonValueWriter.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;
using namespace CXPM::Modules::Serialization;

namespace CXPM::Modules::Serialization::Testing {

struct JsonValueCodecTest : public TestGroup {
  JsonValueCodecTest()
      : TestGroup(
            "JsonValueCodec",
            {
                {"parses and re-renders every scalar alternative",
                 [](TestContext &ctx) {
                   ctx.equal(write_json(parse_json("null")), String("null"));
                   ctx.equal(write_json(parse_json("true")), String("true"));
                   ctx.equal(write_json(parse_json("false")),
                             String("false"));
                   ctx.equal(write_json(parse_json("42")), String("42"));
                   ctx.equal(write_json(parse_json("-7")), String("-7"));
                   ctx.equal(write_json(parse_json("\"hello\"")),
                             String("\"hello\""));
                 }},
                {"parses a double distinctly from an int",
                 [](TestContext &ctx) {
                   auto value = parse_json("3.5");
                   ctx.check(std::holds_alternative<double>(value));
                   ctx.equal(std::get<double>(value), 3.5);
                 }},
                {"decodes string escapes including \\u BMP codepoints",
                 [](TestContext &ctx) {
                   auto value = parse_json(R"("line\nbreak \"quoted\" é")");
                   ctx.check(std::holds_alternative<String>(value));
                   ctx.equal(std::get<String>(value),
                             String("line\nbreak \"quoted\" \xc3\xa9"));
                 }},
                {"round-trips a nested array of objects",
                 [](TestContext &ctx) {
                   String text =
                       R"({"name":"example","sources":["a.cpp","b.cpp"],)"
                       R"("nested":{"enabled":true,"count":2}})";
                   auto value = parse_json(text);
                   ctx.check(std::holds_alternative<Map<String, Value>>(value));

                   auto rendered = write_json(value);
                   auto reparsed = parse_json(rendered);
                   ctx.check(std::holds_alternative<Map<String, Value>>(
                       reparsed));

                   auto &object = std::get<Map<String, Value>>(reparsed);
                   ctx.equal(std::get<String>(object.at("name")),
                             String("example"));
                   auto &sources =
                       std::get<BasicCollection<Value>>(object.at("sources"));
                   ctx.equal(sources.size(), std::size_t{2});
                   ctx.equal(std::get<String>(sources[0]), String("a.cpp"));

                   auto &nested = std::get<Map<String, Value>>(
                       object.at("nested"));
                   ctx.equal(std::get<bool>(nested.at("enabled")), true);
                   ctx.equal(std::get<int>(nested.at("count")), 2);
                 }},
                {"an empty array and empty object round-trip",
                 [](TestContext &ctx) {
                   ctx.equal(write_json(parse_json("[]")), String("[]"));
                   ctx.equal(write_json(parse_json("{}")), String("{}"));
                 }},
                {"whitespace between tokens is ignored",
                 [](TestContext &ctx) {
                   auto value = parse_json(" { \"a\" : 1 ,\n\"b\" : 2 } ");
                   auto &object = std::get<Map<String, Value>>(value);
                   ctx.equal(std::get<int>(object.at("a")), 1);
                   ctx.equal(std::get<int>(object.at("b")), 2);
                 }},
                {"write_json_pretty produces indented, re-parseable output",
                 [](TestContext &ctx) {
                   String text = R"({"name":"example","sources":["a.cpp"]})";
                   auto value = parse_json(text);
                   auto pretty = write_json_pretty(value);

                   ctx.check(pretty.find('\n') != String::npos,
                             "pretty output should contain newlines");
                   ctx.check(pretty.find("  \"name\"") != String::npos,
                             "pretty output should indent object members");

                   auto reparsed = parse_json(pretty);
                   ctx.equal(write_json(reparsed), write_json(value));
                 }},
                {"decodes every single-character escape and a 3-byte UTF-8 "
                 "\\u codepoint (U+20AC, the euro sign)",
                 [](TestContext &ctx) {
                   // A raw string literal so the backslashes reach the JSON parser
                   // literally, rather than being interpreted by the C++ compiler.
                   String text = R"("\b\f\r\t\/ )";
                   text += "\\u20AC\"";
                   auto value = parse_json(text);
                   ctx.equal(std::get<String>(value),
                             String("\b\f\r\t/ \xe2\x82\xac"));
                 }},
                {"a token that doesn't match the expected closing character "
                 "(not simply end-of-input) throws JsonParseException",
                 [](TestContext &ctx) {
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json(R"({"a":1 X})"); });
                 }},
                {"every string-escape error path throws JsonParseException",
                 [](TestContext &ctx) {
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json("\"unterminated"); });
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json("\"trailing backslash\\"); });
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json(R"("\u12")"); });
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json(R"("\uZZZZ")"); });
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json(R"("\q")"); });
                 }},
                {"malformed JSON throws JsonParseException",
                 [](TestContext &ctx) {
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json("{\"a\": }"); });
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json("[1, 2"); });
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json("nul"); });
                   ctx.throws<CXPM::Core::Exceptions::JsonParseException>(
                       [] { parse_json("{\"a\": 1} trailing"); });
                 }},
            }) {}
};

inline static JsonValueCodecTest json_value_codec_test_instance;

} // namespace CXPM::Modules::Serialization::Testing
