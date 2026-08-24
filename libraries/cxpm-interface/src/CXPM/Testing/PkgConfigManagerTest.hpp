#pragma once

#include "CXPM/PkgConfigManager.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Controllers::Testing {

struct PkgConfigManagerTest : public TestGroup {
  PkgConfigManagerTest()
      : TestGroup(
            "PackageConfigManager::emit_configuration",
            {
                {"substitutes name, prefix and version into the .pc "
                 "template",
                 [](TestContext &ctx) {
                   CXPM::PackageDescriptor package;
                   package.name = "example";
                   package.version = "1.2.3";

                   CXPM::ToolchainDescriptor toolchain;
                   toolchain.install_prefix = "/usr/local";

                   auto configuration =
                       CXPM::Controllers::PackageConfigManager::
                           emit_configuration(package, toolchain);

                   ctx.check(configuration.find("prefix=/usr/local") !=
                                  String::npos,
                              "prefix was not substituted");
                   ctx.check(configuration.find("libexample") !=
                                  String::npos,
                              "name was not substituted");
                   ctx.check(configuration.find("Version: 1.2.3") !=
                                  String::npos,
                              "version was not substituted with "
                              "package.version (regression for the "
                              "name/version substitution bug)");
                 }},
            }) {}
};

inline static PkgConfigManagerTest pkg_config_manager_test_instance;

} // namespace CXPM::Controllers::Testing
