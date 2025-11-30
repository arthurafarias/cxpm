#include "Serialization/JsonOutputArchiver.hpp"
#include <Controllers/ClangdManager.hpp>

int main(int argc, char *argv[]) {

  Controllers::CompileCommandDescriptor descriptor{
    .directory = "directory",
    .command = "command",
    .file = "file",
    .output = "output"
  };

  Collection<Controllers::CompileCommandDescriptor> descriptors;

  descriptors.push_back(descriptor);

  Serialization::JsonOutputArchiver ar("compile_commands.json");

  ar % descriptors;

  return 0;
}