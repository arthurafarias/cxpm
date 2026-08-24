#pragma once

// Derives a bidirectional JSON <-> Value codec on top of the token-based JsonOutputArchiver
// (CompilerCommandDescriptor.hpp, JsonOutputArchiver.hpp): this half renders a generic
// CXPM::Core::Containers::Value tree to JSON text. The parsing half lives in JsonValueParser.hpp.
// Together they are what lets package.json/toolchain.json exist as serialization alternatives to
// package.cpp/toolchain.cpp (see docs/SRS-json-manifests.md).

#include "CXPM/Core/Containers/Value.hpp"

#include <iomanip>
#include <ostream>
#include <sstream>
#include <variant>

namespace CXPM::Modules::Serialization {

// The exact std::variant<...> Value derives from. std::visit/std::get require this exact
// specialization (a class merely deriving from std::variant does not itself satisfy the
// "specialization of variant" requirement in every implementation), so every generic-Value
// traversal in this file goes through this alias rather than std::visit(fn, value) directly.
using ValueVariant = CXPM::Core::Containers::Value::BaseType::base_type;

inline void write_json(std::ostream &stream,
                        const CXPM::Core::Containers::Value &value) {
  const auto &variant = static_cast<const ValueVariant &>(value);

  std::visit(
      [&stream](const auto &alternative) {
        using AlternativeType = std::decay_t<decltype(alternative)>;

        if constexpr (std::is_same_v<AlternativeType, std::nullptr_t>) {
          stream << "null";
        } else if constexpr (std::is_same_v<AlternativeType, bool>) {
          stream << (alternative ? "true" : "false");
        } else if constexpr (std::is_same_v<AlternativeType, int> ||
                             std::is_same_v<AlternativeType, double>) {
          stream << alternative;
        } else if constexpr (std::is_same_v<AlternativeType,
                                            CXPM::Core::Containers::String>) {
          stream << std::quoted(alternative.c_str());
        } else if constexpr (std::is_same_v<
                                 AlternativeType,
                                 CXPM::Core::Containers::BasicCollection<
                                     CXPM::Core::Containers::Value>>) {
          stream << "[";
          bool first = true;
          for (const auto &element : alternative) {
            if (!first) {
              stream << ",";
            }
            first = false;
            write_json(stream, element);
          }
          stream << "]";
        } else if constexpr (std::is_same_v<
                                 AlternativeType,
                                 CXPM::Core::Containers::Map<
                                     CXPM::Core::Containers::String,
                                     CXPM::Core::Containers::Value>>) {
          stream << "{";
          bool first = true;
          for (const auto &[key, mapped] : alternative) {
            if (!first) {
              stream << ",";
            }
            first = false;
            stream << std::quoted(key.c_str()) << ":";
            write_json(stream, mapped);
          }
          stream << "}";
        }
      },
      variant);
}

inline CXPM::Core::Containers::String
write_json(const CXPM::Core::Containers::Value &value) {
  std::ostringstream stream;
  write_json(stream, value);
  return CXPM::Core::Containers::String(stream.str());
}

// A human-readable, indented rendering used by `cxpm --generate ... --format json` (see
// docs/SRS-generate.md) so a freshly generated package.json/toolchain.json is pleasant to
// hand-edit. write_json() above stays compact/single-line, since that is what every round-trip
// test and the manifest loader itself actually parse back.
inline void write_json_pretty(std::ostream &stream,
                              const CXPM::Core::Containers::Value &value,
                              std::size_t depth = 0,
                              std::size_t indent_width = 2) {
  const auto &variant = static_cast<const ValueVariant &>(value);
  const std::string indent(depth * indent_width, ' ');
  const std::string child_indent((depth + 1) * indent_width, ' ');

  std::visit(
      [&](const auto &alternative) {
        using AlternativeType = std::decay_t<decltype(alternative)>;

        if constexpr (std::is_same_v<
                          AlternativeType,
                          CXPM::Core::Containers::BasicCollection<
                              CXPM::Core::Containers::Value>>) {
          if (alternative.empty()) {
            stream << "[]";
            return;
          }
          stream << "[\n";
          bool first = true;
          for (const auto &element : alternative) {
            if (!first) {
              stream << ",\n";
            }
            first = false;
            stream << child_indent;
            write_json_pretty(stream, element, depth + 1, indent_width);
          }
          stream << "\n" << indent << "]";
        } else if constexpr (std::is_same_v<
                                 AlternativeType,
                                 CXPM::Core::Containers::Map<
                                     CXPM::Core::Containers::String,
                                     CXPM::Core::Containers::Value>>) {
          if (alternative.empty()) {
            stream << "{}";
            return;
          }
          stream << "{\n";
          bool first = true;
          for (const auto &[key, mapped] : alternative) {
            if (!first) {
              stream << ",\n";
            }
            first = false;
            stream << child_indent << std::quoted(key.c_str()) << ": ";
            write_json_pretty(stream, mapped, depth + 1, indent_width);
          }
          stream << "\n" << indent << "}";
        } else {
          // scalars: identical to the compact writer.
          write_json(stream, value);
        }
      },
      variant);
}

inline CXPM::Core::Containers::String
write_json_pretty(const CXPM::Core::Containers::Value &value,
                  std::size_t indent_width = 2) {
  std::ostringstream stream;
  write_json_pretty(stream, value, 0, indent_width);
  return CXPM::Core::Containers::String(stream.str());
}

} // namespace CXPM::Modules::Serialization
