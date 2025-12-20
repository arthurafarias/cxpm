
#include "Core/Containers/Collection.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Exceptions/RuntimeException.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <utility>

using namespace Core::Containers;

class ParseError : public Core::Exceptions::RuntimeException {
public:
  ParseError(int position)
      : Core::Exceptions::RuntimeException("ParseError at line: {}", position) {
  }
};

enum class TokenType { Literal, Reference, Equals, Alternative, Terminator };

struct Token {
  String value;
  TokenType type;
  size_t start;
  size_t end;
};

Collection<Token> tokenize(String &input) {

  Collection<Token> output;
  Collection<Token> stack;

  for (size_t i = 0; i < input.size(); i++) {
    auto element = input[i];

    switch (element) {

    case '\"': {

      if (stack.empty()) {
        stack.push_front(Token{.value = String{},
                               .type = TokenType::Literal,
                               .start = i,
                               .end = i + 1});
        break;
      }

      switch ((stack.front().type)) {

      case TokenType::Literal:
        output.push_back(stack.front());
        stack.pop_front();
        break;

      case TokenType::Reference:
      case TokenType::Alternative:
      case TokenType::Terminator:
        throw Core::Exceptions::RuntimeException("Invalid token {}", i);
        break;
      case TokenType::Equals:
        break;
      }
      break;
    }

    case '|': {

      if (stack.empty()) {
        output.push_back(Token{.value = String{element},
                               .type = TokenType::Alternative,
                               .start = i,
                               .end = i + 1});
        break;
      }

      throw Core::Exceptions::RuntimeException("Invalid token at {}", i);

      break;
    }

    case '=': {

      if (stack.empty()) {
        output.push_back(Token{.value = String{element},
                               .type = TokenType::Equals,
                               .start = i,
                               .end = i + 1});
        break;
      }

      throw Core::Exceptions::RuntimeException("Invalid token at {}", i);

      break;
    }

    case '<': {

      if (stack.empty()) {
        stack.push_front(Token{.value = String{},
                               .type = TokenType::Reference,
                               .start = i,
                               .end = i + 1});
        break;
      }

      throw Core::Exceptions::RuntimeException("Invalid token at {}", i);
      break;
    }

    case '>': {

      if (stack.empty()) {
        throw Core::Exceptions::RuntimeException("Invalid token at {}", i);
        break;
      }

      if (stack.front().type != TokenType::Reference) {
        throw Core::Exceptions::RuntimeException("Invalid token at {}", i);
        break;
      }

      output.push_back(stack.front());
      stack.pop_front();

      break;
    }

    case ';': {

      if (!stack.empty()) {
        throw Core::Exceptions::RuntimeException("Invalid token at {}", i);
        break;
      }

      output.push_back(Token{.value = String{element},
                             .type = TokenType::Terminator,
                             .start = i,
                             .end = i + 1});

      break;
    }

    case ' ': {

      if (stack.empty()) {
        break;
      }

      switch ((stack.front().type)) {

      case TokenType::Literal:
      case TokenType::Reference:
        stack.front().value.push_back(element);
        stack.front().end = i;
        break;

      case TokenType::Alternative:
      case TokenType::Terminator:
        throw Core::Exceptions::RuntimeException(
            "Invalid expression between {}", i, i + 1);
        break;
      case TokenType::Equals:
        break;
      }
      break;
    }

    case '\n': {

      if (stack.empty()) {
        break;
      }

      throw Core::Exceptions::RuntimeException("Failed to parse at {}", i);
    }

    default: {

      if (stack.empty()) {
        throw Core::Exceptions::RuntimeException("Failed to parse at {}", i);
      }

      switch ((stack.front().type)) {

      case TokenType::Literal:
      case TokenType::Reference:
        stack.front().value.push_back(element);
        stack.front().end = i;
        break;

      case TokenType::Alternative:
      case TokenType::Terminator:
        throw Core::Exceptions::RuntimeException(
            "Invalid expression between {}", i, i + 1);
        break;
      case TokenType::Equals:
        break;
      }
      break;
    }
    }
  }

  return output;
}

struct Expression;

enum class ExpressionType {
  Unknown,
  Sequence,
  Alternative,
  Literal,
  Reference
};

struct Expression {
  ExpressionType type;
  String value;
  Collection<Expression> children;
};

struct Grammar : public Map<String, Expression> {};

Expression parse_term(Collection<Token>::iterator& it,
                      Collection<Token>::iterator end);
Expression parse_expression(Collection<Token>::iterator& it,
                            Collection<Token>::iterator end);

// -------------------------------------------------------------------
// Helper to create leaf nodes
Expression make_literal(const Token &tok) {
  return Expression{ExpressionType::Literal, tok.value, {}};
}

Expression make_reference(const Token &tok) {
  return Expression{ExpressionType::Reference, tok.value, {}};
}

Expression parse_sequence(Collection<Token>::iterator &it,
                          Collection<Token>::iterator end) {
  Expression seq{ExpressionType::Sequence, "", {}};

  // At least one term must follow the '=' or after a '|'
  while (it != end && it->type != TokenType::Alternative &&
         it->type != TokenType::Terminator) {

    seq.children.push_back(parse_term(it, end));
    it++;
  }

  if (seq.children.size() == 1)
    return std::move(seq.children.front());
  return seq;
}

Expression parse_alternative(Collection<Token>::iterator& it,
                             Collection<Token>::iterator end) {
  Expression alt{ExpressionType::Alternative, "", {}};

  while (true) {
    alt.children.push_back(parse_sequence(it, end));

    if (it == end || it->type != TokenType::Alternative)
      break;
    ++it; // skip '|'
  }

  if (alt.children.size() == 1)
    return std::move(alt.children.front());
  return alt;
}

Expression parse_term(Collection<Token>::iterator& it,
                      Collection<Token>::iterator end) {
  auto begin = it;
  if (it->type == TokenType::Literal)
    return make_literal(*it);
  if (it->type == TokenType::Reference)
    return make_reference(*it);

  throw ParseError(it - begin);
}

Expression parse_expression(Collection<Token>::iterator& it,
                            Collection<Token>::iterator end) {
  // `expression` is just an alternative for now
  return parse_alternative(it, end);
}

// ------------------------------------------------------------
// The public API – builds the Grammar map
Grammar parse(Collection<Token> &tokens) {
  Grammar grammar;
  Collection<Token>::iterator it = tokens.begin();
  Collection<Token>::iterator end = tokens.end();

  while (it != end) {
    // ---- definition ------------------------------------
    if (it->type != TokenType::Reference)
      throw ParseError(it - tokens.begin());

    std::string name = it->value; // left‑hand side
    ++it;                         // skip reference

    if (it == end || it->type != TokenType::Equals)
      throw ParseError(it - tokens.begin());
    ++it; // skip '='

    Expression rhs = parse_expression(it, end);

    grammar[name] = std::move(rhs);

    if (it == end)
      break;

    if (it->type != TokenType::Terminator)
      throw ParseError(it - tokens.begin());
    ++it; // skip ';'
  }

  return grammar;
}

int main(int argc, char *argv[]) {
  String str = R"(
<integer> = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
<number> = <number> <integer>
<factor_sum> = <factor> "+" <number>;
<factor_sub> = <factor> "-" <number>;
<expression> = <sum> | <subtraction>;
)";

  try {
    auto tokens = tokenize(str);
    Grammar grammar = parse(tokens);

    // Simple dump – prints the left‑hand side and the structure
    for (const auto &kv : grammar) {
      std::cout << kv.first << " = ";
      std::function<void(const Expression &, int)> print;
      print = [&](const Expression &e, int indent) {
        std::string pad(indent * 2, ' ');
        if (e.type == ExpressionType::Literal)
          std::cout << '"' << e.value << '"';
        else if (e.type == ExpressionType::Reference)
          std::cout << '<' << e.value << '>';
        else if (e.type == ExpressionType::Sequence) {
          for (size_t i = 0; i < e.children.size(); ++i) {
            print(e.children[i], indent);
            if (i + 1 != e.children.size())
              std::cout << " ";
          }
        } else { // Alternative
          for (size_t i = 0; i < e.children.size(); ++i) {
            print(e.children[i], indent);
            if (i + 1 != e.children.size())
              std::cout << " | ";
          }
        }
      };
      print(kv.second, 0);
      std::cout << '\n';
    }

  } catch (const ParseError &ex) {
    std::cerr << "Parse error: " << ex.what() << '\n';
    return EXIT_FAILURE;
  }

  std::cout << "Parsing succeeded.\n";
  return EXIT_SUCCESS;
}
