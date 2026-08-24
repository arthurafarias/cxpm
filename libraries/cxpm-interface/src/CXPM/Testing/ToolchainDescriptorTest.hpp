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
                {"every remaining fluent setter/getter pair round-trips too",
                 [](TestContext &ctx) {
                   auto toolchain =
                       CXPM::Toolchain()
                           .install_prefix_set("/opt/cxpm")
                           .include_directory_prefix_set("-isystem")
                           .include_directories_set({"/opt/cxpm/include"})
                           .object_build_options_set({"-fPIC"})
                           .compiler_options_set({"-pthread"})
                           .archiver_options_set({"rcs"})
                           .linker_options_set({"-shared"})
                           .link_directory_prefix_set("-L")
                           .link_library_prefix_set("-l")
                           .source_specifier_prefix_set("-c")
                           .object_specifier_prefix_set("-o")
                           .link_directories_set({"/opt/cxpm/lib"})
                           .shared_object_prefix_set("cxpm-")
                           .shared_object_suffix_set(".dylib")
                           .archive_prefix_set("cxpm-")
                           .archive_suffix_set(".lib")
                           .executable_prefix_set("run-")
                           .executable_suffix_set(".exe")
                           .object_prefix_set("obj-")
                           .object_suffix_set(".obj");

                   ctx.equal(toolchain.install_prefix_get(),
                             String("/opt/cxpm"));
                   ctx.equal(toolchain.include_directory_prefix_get(),
                             String("-isystem"));
                   ctx.equal(toolchain.include_directories_get()[0],
                             String("/opt/cxpm/include"));
                   ctx.equal(toolchain.object_build_options_get()[0],
                             String("-fPIC"));
                   ctx.equal(toolchain.compiler_options_get()[0],
                             String("-pthread"));
                   ctx.equal(toolchain.archiver_options_get()[0],
                             String("rcs"));
                   ctx.equal(toolchain.linker_options_get()[0],
                             String("-shared"));
                   ctx.equal(toolchain.link_directory_prefix_get(),
                             String("-L"));
                   ctx.equal(toolchain.link_library_prefix_get(),
                             String("-l"));
                   ctx.equal(toolchain.source_specifier_prefix_get(),
                             String("-c"));
                   ctx.equal(toolchain.object_specifier_prefix_get(),
                             String("-o"));
                   ctx.equal(toolchain.link_directories_get()[0],
                             String("/opt/cxpm/lib"));
                   ctx.equal(toolchain.shared_object_prefix_get(),
                             String("cxpm-"));
                   ctx.equal(toolchain.shared_object_suffix_get(),
                             String(".dylib"));
                   ctx.equal(toolchain.archive_prefix_get(), String("cxpm-"));
                   ctx.equal(toolchain.archive_suffix_get(), String(".lib"));
                   ctx.equal(toolchain.executable_prefix_get(),
                             String("run-"));
                   ctx.equal(toolchain.executable_suffix_get(),
                             String(".exe"));
                   ctx.equal(toolchain.object_prefix_get(), String("obj-"));
                   ctx.equal(toolchain.object_suffix_get(), String(".obj"));
                 }},
            }) {}
};

inline static ToolchainDescriptorTest toolchain_descriptor_test_instance;

} // namespace CXPM::Testing
