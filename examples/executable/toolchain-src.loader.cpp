
    #include <CXPM/ToolchainDescriptor.hpp>
    using namespace CXPM;
    extern ToolchainDescriptor toolchain;
    extern "C" const ToolchainDescriptor* get_toolchain()  { return &toolchain; }
  