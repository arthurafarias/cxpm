#include "Controllers/PkgConfigManager.hpp"
#include "Models/TargetDescriptor.hpp"
#include <Models/ProjectDescriptor.hpp>

auto example = Models::TargetDescriptor()
                   .name_set("example-executable")
                   .version_set("1.0.0")
                   .type_set("executable")
                   .sources_append({"src/main.cpp", "src/source0.cpp",
                                    "src/source1.cpp", "src/source2.cpp"})
                   .sources_append({"src/source3.cpp"})
                   .options_append({"-fPIE", "-fstack-protector-all"})
                   .link_libraries_append({"m"})
                   .include_directories_append({})
                   .dependencies_append("gstreamer-1.0")
                   .create();

auto project = Models::ProjectDescriptor().add(example).create();