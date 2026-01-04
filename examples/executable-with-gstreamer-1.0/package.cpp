#include <CXPM/Models/Project.hpp>
#include <CXPM/Models/Target.hpp>

using namespace CXPM::Models;

auto example = Target()
                   .name_set("example-executable-with-gstreamer-1.0")
                   .version_set("1.0.0")
                   .type_set("executable")
                   .sources_append({"src/main.cpp"})
                   .options_append({"-fPIE", "-fstack-protector-all"})
                   .link_libraries_append({"m"})
                   .include_directories_append({"src"})
                   .dependencies_append("gstreamer-1.0")
                   .create();

auto project = Project().add(example).create();