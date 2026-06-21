#pragma once

#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace Models {

  template<typename DerivedType>
struct PackageInterface {
  virtual DerivedType& name_set(const String &) = 0;
  virtual const String &name_get() const = 0;
  virtual DerivedType& version_set(const String &) = 0;
  virtual const String &version_get() const = 0;
  virtual DerivedType& install_prefix_set(const String &) = 0;
  virtual const String &install_prefix_get() const = 0;
  virtual DerivedType& include_directories_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &include_directories_get() const = 0;
  virtual DerivedType& link_directories_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &link_directories_get() const = 0;
  virtual DerivedType& link_libraries_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &link_libraries_get() const = 0;
  virtual DerivedType& options_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &options_get() const = 0;
  virtual const String& project_path_get() = 0;
  virtual DerivedType& project_path_set(const String&) = 0;
};
} // namespace Models