#include "Modules/Serialization/JSON/JSONOutputArchiver.hpp"
#include <Controllers/ClangdManager.hpp>


int main(int argc, char *argv[]) {

  Models::CompileCommandDescriptor descriptor{.directory = "directory",
                                              .command = "command",
                                              .file = "file",
                                              .output = "output"};

  Collection<Models::CompileCommandDescriptor> descriptors;

  descriptors.push_back(descriptor);

  auto stream = std::ofstream("compile_commands.json");
  Modules::Serialization::JSON::JSONOutputArchiver ar(stream);

  ar % descriptors;

  return 0;
}