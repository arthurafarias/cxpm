#pragma once

// The read-side counterpart to JsonOutputArchiver: wraps an std::istream, reads it to
// completion and parses it (via JsonValueParser) into a CXPM::Core::Containers::Value tree
// once, at construction time, rather than mirroring JsonOutputArchiver's token-by-token
// streaming protocol on the way in — see JsonValueParser.hpp for why a DOM-style parse was
// chosen over a fully symmetric token reader.

#include "CXPM/Modules/Serialization/AbstractInputArchiver.hpp"
#include "CXPM/Modules/Serialization/JsonValueParser.hpp"

#include <iterator>
#include <sstream>

namespace CXPM::Modules::Serialization {

struct JsonInputArchiver : public virtual AbstractInputArchiver {
  explicit JsonInputArchiver(std::istream &stream)
      : AbstractInputArchiver(stream) {
    std::ostringstream buffer;
    buffer << stream_get().rdbuf();
    value_ = parse_json(CXPM::Core::Containers::String(buffer.str()));
  }

  [[nodiscard]] const CXPM::Core::Containers::Value &value() const {
    return value_;
  }

private:
  CXPM::Core::Containers::Value value_;
};

} // namespace CXPM::Modules::Serialization
