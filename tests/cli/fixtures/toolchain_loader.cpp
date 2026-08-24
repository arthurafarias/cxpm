// The same generated loader stub ToolchainManager::build_toolchain_plugin() writes at build
// time (see ToolchainLoaderSource in ToolchainManager.hpp), checked in here so the sandbox
// integration tests have a real, buildable toolchain plugin .so fixture to point
// cxpm-descriptor-sandbox at without needing to invoke the full cxpm CLI first.
#include <CXPM/ToolchainDescriptor.hpp>
using namespace CXPM;
extern ToolchainDescriptor toolchain;
extern "C" const ToolchainDescriptor *get_toolchain() { return &toolchain; }
