#pragma once

// The parsing half of the JSON <-> Value codec (see JsonValueWriter.hpp for the writing half
// and docs/SRS-json-manifests.md for why this exists): a small hand-rolled recursive-descent
// JSON parser producing a generic CXPM::Core::Containers::Value tree. Deliberately parses to a
// tree first rather than attempting a fully symmetric token-driven reader matching
// JsonOutputArchiver's write-side token DSL (ObjectStartToken/PairToken/...) one token at a
// time — that would require lookahead-matching a fixed emission order against arbitrary input
// structure, which is unnecessary complexity when a DOM-style parse-then-convert (see
// JsonManifest.hpp for the descriptor-level from_json conversions built on top of this) gets the
// same result with far less code.

#include "CXPM/Core/Containers/Value.hpp"
#include "CXPM/Core/Exceptions/JsonParseException.hpp"

#include <cstdlib>
#include <string>

namespace CXPM::Modules::Serialization {

class JsonValueParser {
public:
  static CXPM::Core::Containers::Value
  parse(const CXPM::Core::Containers::String &text) {
    JsonValueParser parser(text);
    parser.skip_whitespace();
    auto result = parser.parse_value();
    parser.skip_whitespace();
    if (!parser.at_end()) {
      throw CXPM::Core::Exceptions::JsonParseException(
          "unexpected trailing content at offset {} while parsing JSON",
          parser.position_);
    }
    return result;
  }

private:
  explicit JsonValueParser(const CXPM::Core::Containers::String &text)
      : text_(text) {}

  [[nodiscard]] bool at_end() const { return position_ >= text_.size(); }

  [[nodiscard]] char peek() const {
    if (at_end()) {
      throw CXPM::Core::Exceptions::JsonParseException(
          "unexpected end of input while parsing JSON");
    }
    return text_[position_];
  }

  char advance() { return text_[position_++]; }

  void expect(char expected) {
    if (at_end() || peek() != expected) {
      throw CXPM::Core::Exceptions::JsonParseException(
          "expected '{}' at offset {} while parsing JSON", expected,
          position_);
    }
    advance();
  }

  void skip_whitespace() {
    while (!at_end()) {
      char c = peek();
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        advance();
      } else {
        break;
      }
    }
  }

  void expect_literal(const char *literal) {
    for (const char *ch = literal; *ch != '\0'; ++ch) {
      expect(*ch);
    }
  }

  CXPM::Core::Containers::Value parse_value() {
    if (at_end()) {
      throw CXPM::Core::Exceptions::JsonParseException(
          "unexpected end of input while parsing JSON");
    }
    switch (peek()) {
    case '{':
      return parse_object();
    case '[':
      return parse_array();
    case '"':
      return CXPM::Core::Containers::Value(parse_string());
    case 't':
      expect_literal("true");
      return CXPM::Core::Containers::Value(true);
    case 'f':
      expect_literal("false");
      return CXPM::Core::Containers::Value(false);
    case 'n':
      expect_literal("null");
      return CXPM::Core::Containers::Value(nullptr);
    default:
      return parse_number();
    }
  }

  CXPM::Core::Containers::Value parse_object() {
    expect('{');
    CXPM::Core::Containers::Map<CXPM::Core::Containers::String,
                                CXPM::Core::Containers::Value>
        object;
    skip_whitespace();
    if (!at_end() && peek() == '}') {
      advance();
      return CXPM::Core::Containers::Value(object);
    }
    while (true) {
      skip_whitespace();
      auto key = parse_string();
      skip_whitespace();
      expect(':');
      skip_whitespace();
      object[key] = parse_value();
      skip_whitespace();
      if (!at_end() && peek() == ',') {
        advance();
        continue;
      }
      expect('}');
      break;
    }
    return CXPM::Core::Containers::Value(object);
  }

  CXPM::Core::Containers::Value parse_array() {
    expect('[');
    CXPM::Core::Containers::BasicCollection<CXPM::Core::Containers::Value>
        array;
    skip_whitespace();
    if (!at_end() && peek() == ']') {
      advance();
      return CXPM::Core::Containers::Value(array);
    }
    while (true) {
      skip_whitespace();
      array.push_back(parse_value());
      skip_whitespace();
      if (!at_end() && peek() == ',') {
        advance();
        continue;
      }
      expect(']');
      break;
    }
    return CXPM::Core::Containers::Value(array);
  }

  static void append_utf8(std::string &out, unsigned int codepoint) {
    if (codepoint <= 0x7F) {
      out += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) {
      out += static_cast<char>(0xC0 | (codepoint >> 6));
      out += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
      out += static_cast<char>(0xE0 | (codepoint >> 12));
      out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
      out += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
  }

  CXPM::Core::Containers::String parse_string() {
    expect('"');
    std::string result;
    while (true) {
      if (at_end()) {
        throw CXPM::Core::Exceptions::JsonParseException(
            "unterminated string while parsing JSON");
      }
      char c = advance();
      if (c == '"') {
        break;
      }
      if (c != '\\') {
        result += c;
        continue;
      }
      if (at_end()) {
        throw CXPM::Core::Exceptions::JsonParseException(
            "unterminated escape sequence while parsing JSON");
      }
      char escaped = advance();
      switch (escaped) {
      case '"':
        result += '"';
        break;
      case '\\':
        result += '\\';
        break;
      case '/':
        result += '/';
        break;
      case 'b':
        result += '\b';
        break;
      case 'f':
        result += '\f';
        break;
      case 'n':
        result += '\n';
        break;
      case 'r':
        result += '\r';
        break;
      case 't':
        result += '\t';
        break;
      case 'u': {
        // Basic Multilingual Plane only: surrogate pairs are not decoded.
        unsigned int codepoint = 0;
        for (int i = 0; i < 4; ++i) {
          if (at_end()) {
            throw CXPM::Core::Exceptions::JsonParseException(
                "truncated \\u escape while parsing JSON");
          }
          char hex = advance();
          codepoint <<= 4;
          if (hex >= '0' && hex <= '9') {
            codepoint |= static_cast<unsigned int>(hex - '0');
          } else if (hex >= 'a' && hex <= 'f') {
            codepoint |= static_cast<unsigned int>(hex - 'a' + 10);
          } else if (hex >= 'A' && hex <= 'F') {
            codepoint |= static_cast<unsigned int>(hex - 'A' + 10);
          } else {
            throw CXPM::Core::Exceptions::JsonParseException(
                "invalid \\u escape digit while parsing JSON");
          }
        }
        append_utf8(result, codepoint);
        break;
      }
      default:
        throw CXPM::Core::Exceptions::JsonParseException(
            "invalid escape sequence '\\{}' while parsing JSON", escaped);
      }
    }
    return CXPM::Core::Containers::String(result);
  }

  CXPM::Core::Containers::Value parse_number() {
    std::size_t start = position_;
    if (!at_end() && peek() == '-') {
      advance();
    }
    if (at_end() || !std::isdigit(static_cast<unsigned char>(peek()))) {
      throw CXPM::Core::Exceptions::JsonParseException(
          "invalid number at offset {} while parsing JSON", start);
    }
    while (!at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
      advance();
    }
    bool is_floating_point = false;
    if (!at_end() && peek() == '.') {
      is_floating_point = true;
      advance();
      while (!at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
      }
    }
    if (!at_end() && (peek() == 'e' || peek() == 'E')) {
      is_floating_point = true;
      advance();
      if (!at_end() && (peek() == '+' || peek() == '-')) {
        advance();
      }
      while (!at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
      }
    }
    std::string token = text_.substr(start, position_ - start);
    if (is_floating_point) {
      return CXPM::Core::Containers::Value(std::strtod(token.c_str(), nullptr));
    }
    return CXPM::Core::Containers::Value(std::atoi(token.c_str()));
  }

  CXPM::Core::Containers::String text_;
  std::size_t position_ = 0;
};

inline CXPM::Core::Containers::Value
parse_json(const CXPM::Core::Containers::String &text) {
  return JsonValueParser::parse(text);
}

} // namespace CXPM::Modules::Serialization
