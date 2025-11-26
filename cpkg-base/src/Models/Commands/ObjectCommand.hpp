#pragma once

#include <Models/BasicTarget.hpp>
#include <Models/BasicToolchain.hpp>
#include <Models/Commands/BasicCommand.hpp>

namespace Models::Commands {

struct ObjectCommand : public BasicCommand {

  static inline BasicCommand from(const BasicTarget &package,
                                  const BasicToolchain &toolchain) {

    std::deque<std::string> arguments;

    arguments.push_back("-fPIC");

    for (auto option : package.options) {
      arguments.push_back(std::get<Core::Containers::String>(option));
    }

    for (auto link_directory : package.link_directories) {
      arguments.push_back("-L");
      arguments.push_back(std::get<Core::Containers::String>(link_directory));
    }

    for (auto include_directory : package.include_directories) {
      arguments.push_back("-I");
      arguments.push_back(std::get<Core::Containers::String>(include_directory));
    }

    for (auto link_library : package.link_libraries) {
      arguments.push_back("-l");
      arguments.push_back(std::get<Core::Containers::String>(link_library));
    }

    arguments.push_back("-o");
    arguments.push_back(std::get<Core::Containers::String>(package.name) + ".o");

    for (auto source : package.sources) {
      auto object = std::get<Core::Containers::String>(source) + ".o";
      arguments.push_back(object);
    }

    return {.executable = std::get<Core::Containers::String>(toolchain.compiler_executable),
            .arguments = arguments};
  }

  static inline BasicCommand from(const std::string &path,
                                  const BasicTarget &package,
                                  const BasicToolchain &toolchain) {

    std::deque<std::string> arguments;

    arguments.push_back("-fPIC");

    for (auto option : package.options) {
      arguments.push_back(std::get<Core::Containers::String>(option));
    }

    for (auto link_directory : package.link_directories) {
      auto __element = std::get<Core::Containers::String>(link_directory);

      if (__element == "") {
        continue;
      }

      arguments.push_back("-L");
      arguments.push_back(__element);
    }

    for (auto include_directory : package.include_directories) {

      auto __element = std::get<Core::Containers::String>(include_directory);

      if (__element == "") {
        continue;
      }

      arguments.push_back("-I");
      arguments.push_back(__element);
    }

    for (auto link_library : package.link_libraries) {

      auto __element = std::get<Core::Containers::String>(link_library);

      if (__element == "") {
        continue;
      }

      arguments.push_back("-l");
      arguments.push_back(__element);
    }

    arguments.push_back("-c");
    arguments.push_back(path);

    arguments.push_back("-o");
    arguments.push_back(path + ".o");

    return {.executable = std::get<Core::Containers::String>(toolchain.compiler_executable),
            .arguments = arguments};
  }
};
} // namespace Models::Commands