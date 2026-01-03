#include <CXPM/Views/ApplicationView.hpp>
#include <CXPM/Controllers/ProjectManager.hpp>
#include <CXPM/Controllers/ToolchainManager.hpp>
#include <CXPM/Core/Containers/Collection.hpp>
#include <CXPM/Core/Containers/String.hpp>
#include <CXPM/Core/Exceptions/RuntimeException.hpp>
#include <CXPM/Core/Logging/LoggerManager.hpp>
#include <CXPM/Utils/Unix/EnvironmentManager.hpp>

#include <cstdlib>
#include <exception>

#include <dlfcn.h>

using namespace CXPM;

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