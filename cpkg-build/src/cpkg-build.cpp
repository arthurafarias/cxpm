#include <Compiler.hpp>
#include <Package.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef CPKG_BUILD_HEADERS_PATH
#define CPKG_BUILD_HEADERS_PATH "./"
#endif

#ifndef CPKG_BUILD_INSTALL_HEADERS_PATH
#define CPKG_BUILD_INSTALL_HEADERS_PATH "/usr/share/cpkg-build/private-headers/"
#endif

#ifndef CPKG_BASE_HEADERS_PATH
#define CPKG_BASE_HEADERS_PATH "./"
#endif

#ifndef CPKG_BASE_INSTALL_HEADERS_PATH
#define CPKG_BASE_INSTALL_HEADERS_PATH "/usr/share/cpkg-base/private-headers/"
#endif

Package builder = {.name = "project-manifest",
                   .type = "shared-library",
                   .dependencies = {"cpkg-base"},
                   .include_directories = {"-I" CPKG_BUILD_INSTALL_HEADERS_PATH,
                                           "-I" CPKG_BUILD_HEADERS_PATH,
                                           "-I" CPKG_BASE_HEADERS_PATH,
                                           "-I" CPKG_BASE_INSTALL_HEADERS_PATH},
                   .sources = {"package.cpp", "package.loader.cpp"}};

const std::string _package_loader_cpp_source_ = R"(
#include <Compiler.hpp>
#include <Package.hpp>
#include <Toolchain.hpp>
#include <Variant.hpp>
#include <Project.hpp>

extern Project project;

extern "C" const Project* get_project()  { return &project; }

)";

int print_usage() {
  std::cout << R"(
Usage: cpkg-build [options]
    --help: print usage
    --build: build project
    --generate: generate project
)";
  return 0;
}


#include <map>
#include <functional>

int generate_project(std::string path) {

  // compile package.cpp linking cpkg-base library that will provide main.cpp
  // and produce the executable
  {
    // generate main
    std::ofstream _package_main_("package.loader.cpp",
                                 std::ios_base::trunc | std::ios_base::out);
    _package_main_ << _package_loader_cpp_source_;
    _package_main_.flush();
  }

  Compiler compiler;
  compiler.build(builder);

  return 0;
}

std::deque<std::string> split(std::string haystack, std::string needle) {
  std::deque<std::string> splitted;
  int position = 0;
  int position_last = 0;

  while (position < haystack.size()) {  
    position = haystack.find(needle, position);
    splitted.push_back(haystack.substr(position_last, position));
  }

  return splitted;
}

int main(int argc, char *argv[]) {

  std::deque<std::string> args(argv, argv + argc);

  std::map<std::string, std::function<void(const std::deque<std::string>&)>> program_options = {
    { "-h|--help", [](const auto& value) -> std::string { return ""; } }
  };

  for (const auto& descriptor : program_options) {

    auto options = split(descriptor.first, "|");

    for (const auto& option : options) {
      
      auto it = std::find_if(args.begin(), args.end(), [option](const auto& arg){ return option == arg; });

      if (it != args.end()) {
        // args
      }
    }
  }

  auto build_path = std::filesystem::current_path();

  if (args.size() == 1) {
    args.push_back("--help");
  }

  if (std::find_if(args.begin(), args.end(), [](auto str) {
        return str == "--path";
      }) != args.end()) {
  }

  if (std::find_if(args.begin(), args.end(), [](auto str) {
        return str == "--generate";
      }) != args.end()) {
    return generate_project(build_path);
  }

  auto include_argument = std::find_if(
      args.begin(), args.end(), [](auto str) { return str == "--include"; });
  if (include_argument < args.end()) {
    auto include_argument_parameter = include_argument + 1;
    if (include_argument_parameter < args.end()) {
      builder.include_directories.push_back(*include_argument_parameter);
    }
  }

  if (std::find_if(args.begin(), args.end(),
                   [](auto str) { return str == "--clean"; }) != args.end()) {

    generate_project(build_path);

    if (system("./.project-build --clean") != 0) {
      throw std::runtime_error("Failed to clean current project");
    }

    std::filesystem::remove_all(
        std::filesystem::current_path().append("./package.cpp.o"));
    std::filesystem::remove_all(
        std::filesystem::current_path().append("./package.loader.cpp.o"));

    return 0;
  }

  if (args[1] == "--build") {
    generate_project(build_path);

    if (system("./.project-build --build") != 0) {
      throw std::runtime_error("Failed to build current project");
    }

    return 0;
  }

  if (args[1] == "--help") {
    return print_usage();
  }

  return 1;
}