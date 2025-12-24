#pragma once

#include <Core/Containers/String.hpp>

using namespace Core::Containers;

template <typename DerivedType> struct ShellCommandBuilder {
public:
  ShellCommandBuilder(const String &path) {}

  DerivedType &path_set(const String &value) {
    path = value;
    return *this;
  }

  const String &path_get() { return path; }

  DerivedType &argument_add(const String &value) {
    arguments.push_back(value);
    return *this;
  }

  DerivedType &argument_add(const String &key, const String &value) {
    arguments.push_back(key + " " + value);
    return *this;
  }

  const Collection<String> &arguments_get() { return arguments; }

  virtual const String command_get() {
    return path + " " + String::join(arguments, " ");
  }

private:
  String path;
  Collection<String> arguments;
};