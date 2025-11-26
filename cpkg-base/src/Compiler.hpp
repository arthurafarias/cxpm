#pragma once

#include "Toolchain.hpp"
#include <AbstractCompiler.hpp>
#include <Variant.hpp>

#include <deque>
#include <filesystem>
#include <iostream>

class Compiler {
public:
  virtual int clean(const Package &package) {
    auto output_sources = _get_output_sources(package);

    for (auto output : output_sources) {
      std::cout << "Cleaning source..." << std::endl;
      std::filesystem::remove_all(output);
    }

    return 0;
  };

  virtual int generate(const Package &package) { return 0; };

  virtual std::deque<std::string> _get_output_sources(const Package &package) {

    std::deque<std::string> outputs;

    for (const auto &source_descriptor : package.sources) {
      auto source_url = std::get<std::string>(source_descriptor);
      auto output_url = source_url + ".o";
      outputs.push_back(output_url);
    }

    return outputs;
  }

  virtual int build(Package package,
                    const Toolchain &toolchain = DefaultToolchain) {

    std::string options;
    std::string link_libraries;
    std::string include_directories;
    std::deque<std::string> outputs;

    // find project dependencies
    for (const auto &dependency : package.dependencies) {

      // where package.cpp will be stored and how to load them all
      // Wrap Dependency into a Package
      // Package pkg = {
      //   .include_directories =
      // }
      // this is the point where it fails and I have to solve...
      // the point is, when trying to reference package, a package descriptor in
      // another language should be created or it should be changed to a header
      // hpp and declared inline. But the problem here is..

      // 
    }

    if (std::get<std::string>(package.type) == "shared-library") {
      package.options.emplace_back("-fPIC");
      package.options.emplace_back("-shared");
    }

    for (const auto &opt : package.options) {
      options = + " " + std::get<std::string>(opt);
    }

    for (const auto &include : package.include_directories) {
      include_directories += " " + std::get<std::string>(include);
    }

    for (const auto &library : package.link_libraries) {
      link_libraries += " " + std::get<std::string>(library);
    }

    for (const auto &source_descriptor : package.sources) {
      auto source_url = std::get<std::string>(source_descriptor);
      auto output_url = source_url + ".o";

      std::string compile_command =
          "g++" + options + " " + include_directories + " " + link_libraries +
          " -o " + output_url + " " + "-c" + " " + source_url;

      outputs.push_back(output_url);

      std::cout << compile_command << std::endl;

      if (system(compile_command.c_str()) != 0) {
        throw std::runtime_error(std::string("Failed to compile ") +
                                 source_url);
      }
      
    }

    std::string compile_output = "g++" + options + " " + include_directories +
                                 " " + link_libraries + " -o " +
                                 std::get<std::string>(package.name);

    for (auto partial_output : outputs) {
      compile_output += " " + partial_output;
    }

    std::cout << compile_output << std::endl;

    if (system(compile_output.c_str()) != 0) {
      throw "Failed to compile output";
    }

    return 0;
  }
};