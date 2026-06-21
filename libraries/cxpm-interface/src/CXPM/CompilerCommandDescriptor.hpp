#pragma once

#include "CXPM/Modules/Serialization/ObjectEndToken.hpp"
#include "CXPM/Modules/Serialization/ObjectStartToken.hpp"
#include "CXPM/Modules/Serialization/PairToken.hpp"
#include "CXPM/Modules/Serialization/SeparatorToken.hpp"
#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;
using namespace CXPM::Modules::Serialization;

namespace Models {
struct CompileCommandDescriptor {
  String directory;
  String command;
  String file;
  String output;

  String stdout;
  String stderr;
};
} // namespace Models

namespace CXPM::Modules::Serialization {
template <typename Archiver>
inline Archiver &
operator%(Archiver &ar,
          const ValueToken<const Models::CompileCommandDescriptor> &token) {
  auto &descriptor = token.value_get();

  ar % ObjectStartToken{"CompilerCommandDescriptor"};

  ar % PairToken{"directory", descriptor.directory};
  ar % SeparatorToken{};

  ar % PairToken{"command", descriptor.command};
  ar % SeparatorToken{};

  ar % PairToken{"file", descriptor.file};
  ar % SeparatorToken{};

  ar % PairToken{"output", descriptor.output};
  ar % SeparatorToken{};

  ar % PairToken{"stdout", descriptor.stdout};
  ar % SeparatorToken{};
  
  ar % PairToken{"stderr", descriptor.stderr};
  ar % ObjectEndToken{"CompilerCommandDescriptor"};

  return ar;
}
} // namespace CXPM::Modules::Serialization