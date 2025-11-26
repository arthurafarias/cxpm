#pragma once

#include <Variant.hpp>
#include <Utils/Print.hpp>

class Package {
public:
  Variant name;
  Variant version;
  Variant type;
  std::deque<Variant> dependencies;
  std::deque<Variant> include_directories;
  std::deque<Variant> sources;
  std::deque<Variant> options;
  std::deque<Variant> link_libraries;

  template<typename Archiver>
  void serialize(const Archiver& ar) {
    ar & Archiver::key("package");
    ar & Archiver::named_value("name", name);
    ar & Archiver::named_value("version", version);
    ar & Archiver::named_value("type", type);
    ar & Archiver::named_value("dependencies", dependencies);
    ar & Archiver::named_value("iclude_directories", include_directories);
    ar & Archiver::named_value("sources", sources);
    ar & Archiver::named_value("options", options);
    ar & Archiver::named_value("link_libraries", link_libraries);
  }
};

template<>
inline std::ostream& print<Package>(std::ostream& os, Package value) {
  return os;
}

// extern Package package;