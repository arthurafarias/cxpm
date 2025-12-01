#include "Models/Toolchain.hpp"
#include "Modules/Serialization/JsonOutputArchiver.hpp"
#include "Views/ApplicationView.hpp"
#include <Controllers/ProjectManager.hpp>
#include <Controllers/ToolchainManager.hpp>
#include <Core/Containers/Collection.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Exceptions/RuntimeException.hpp>
#include <Core/Logging/LoggerManager.hpp>
#include <Utils/Unix/EnvironmentManager.hpp>

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include <dlfcn.h>

int main(int argc, char *argv[]) {
  Views::ApplicationView app(argc, argv);
  try {
    return app.run();
  } catch (std::exception &ex) {
    Core::Logging::LoggerManager::error("Failed: {}", ex.what());
    app.print_usage();
    return EXIT_FAILURE;
  }
}