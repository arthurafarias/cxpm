#pragma once

#include <CXPM/Core/Containers/String.hpp>

using namespace CXPM::Core::Containers;

namespace CXPM::Models {
template <typename DerivedType> struct ToolchainDescriptorInterface {
  virtual DerivedType &name_set(const String &) = 0;
  virtual const String &name_get() const = 0;

  virtual DerivedType &version_set(const String &) = 0;
  virtual const String &version_get() const = 0;

  virtual DerivedType &language_set(const String &) = 0;
  virtual const String &language_get() const = 0;

  virtual DerivedType &install_prefix_set(const String &) = 0;
  virtual const String &install_prefix_get() const = 0;

  virtual DerivedType &
  include_directory_prefix_set(const String &) = 0;
  virtual const String &include_directory_prefix_get() const = 0;

  virtual DerivedType &include_directories_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &include_directories_get() const = 0;

  virtual DerivedType &compiler_executable_set(const String &) = 0;
  virtual const String &compiler_executable_get() const = 0;

  virtual DerivedType &compiler_options_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &compiler_options_get() const = 0;

  virtual DerivedType &archiver_executable_set(const String &) = 0;
  virtual const String &archiver_executable_get() const = 0;

  virtual DerivedType &archiver_options_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &archiver_options_get() const = 0;

  virtual DerivedType &linker_executable_set(const String &) = 0;
  virtual const String &linker_executable_get() const = 0;

  virtual DerivedType &linker_options_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &linker_options_get() const = 0;

  virtual DerivedType &link_directory_prefix_set(const String &) = 0;
  virtual const String &link_directory_prefix_get() const = 0;

  virtual DerivedType &link_library_prefix_set(const String &) = 0;
  virtual const String &link_library_prefix_get() const = 0;

  virtual DerivedType &
  source_specifier_prefix_set(const String &) = 0;
  virtual const String &source_specifier_prefix_get() const = 0;

  virtual DerivedType &
  object_specifier_prefix_set(const String &) = 0;
  virtual const String &object_specifier_prefix_get() const = 0;

  virtual DerivedType &link_directories_set(const BasicCollection<String> &) = 0;
  virtual const BasicCollection<String> &link_directories_get() const = 0;

  virtual DerivedType& shared_object_prefix_set(const String& value) = 0;
  virtual const String& shared_object_prefix_get() const = 0;

  virtual DerivedType& shared_object_suffix_set(const String& value) = 0;
  virtual const String& shared_object_suffix_get() const = 0;


  virtual DerivedType& archive_prefix_set(const String& value) = 0;
  virtual const String& archive_prefix_get() const = 0;

  virtual DerivedType& archive_suffix_set(const String& value) = 0;
  virtual const String& archive_suffix_get() const = 0;


  virtual DerivedType& executable_prefix_set(const String& value) = 0;
  virtual const String& executable_prefix_get() const = 0;

  virtual DerivedType& executable_suffix_set(const String& value) = 0;
  virtual const String& executable_suffix_get() const = 0;


  virtual DerivedType& object_prefix_set(const String& value) = 0;
  virtual const String& object_prefix_get() const = 0;

  virtual DerivedType& object_suffix_set(const String& value) = 0;
  virtual const String& object_suffix_get() const = 0;

};
} // namespace CXPM::Models