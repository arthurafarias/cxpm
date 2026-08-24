#pragma once

#include "CXPM/Testing/TestGroup.hpp"
#include "CXPM/Toolchain.hpp"

using namespace CXPM::Core::Containers;

namespace CXPM::Testing {

struct ToolchainDescriptorTest : public TestGroup {
  ToolchainDescriptorTest()
      : TestGroup(
            "ToolchainDescriptor",
            {
                {"default prefixes and suffixes match the documented "
                 "POSIX-aligned conventions",
                 [](TestContext &ctx) {
                   CXPM::ToolchainDescriptor descriptor;
                   ctx.equal(descriptor.include_directory_prefix,
                             String("-I"));
                   ctx.equal(descriptor.link_directory_prefix, String("-L"));
                   ctx.equal(descriptor.link_library_prefix, String("-l"));
                   ctx.equal(descriptor.shared_object_prefix, String("lib"));
                   ctx.equal(descriptor.shared_object_suffix, String(".so"));
                   ctx.equal(descriptor.archive_prefix, String("lib"));
                   ctx.equal(descriptor.archive_suffix, String(".a"));
                   ctx.equal(descriptor.executable_prefix, String(""));
                   ctx.equal(descriptor.executable_suffix, String(""));
                   ctx.equal(descriptor.object_prefix, String(""));
                   ctx.equal(descriptor.object_suffix, String(".o"));
                 }},
                {"the fluent Toolchain builder round-trips every field "
                 "through its setter/getter pair",
                 [](TestContext &ctx) {
                   auto toolchain =
                       CXPM::Toolchain()
                           .name_set("g++")
                           .version_set("generic")
                           .language_set("c++")
                           .compiler_executable_set("/usr/bin/g++")
                           .linker_executable_set("/usr/bin/g++")
                           .archiver_executable_set("/usr/bin/ar");

                   ctx.equal(toolchain.name_get(), String("g++"));
                   ctx.equal(toolchain.version_get(), String("generic"));
                   ctx.equal(toolchain.language_get(), String("c++"));
                   ctx.equal(toolchain.compiler_executable_get(),
                             String("/usr/bin/g++"));
                   ctx.equal(toolchain.linker_executable_get(),
                             String("/usr/bin/g++"));
                   ctx.equal(toolchain.archiver_executable_get(),
                             String("/usr/bin/ar"));
                 }},
            }) {}
};

inline static ToolchainDescriptorTest toolchain_descriptor_test_instance;

} // namespace CXPM::Testing
