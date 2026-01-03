#include <CXPM/Models/Project.hpp>
#include <CXPM/Models/Target.hpp>

using namespace CXPM::Models;

auto target0 = Target()
                   .name_set("example-static-object")
                   .version_set("1.0.0")
                   .type_set("static-library")
                   .sources_append({"src/library.cpp"})
                   .options_append({"-fPIC", "-fstack-protector-all"})
                   .link_libraries_append({"m"})
                   .include_directories_append({"src"})
                   .create();

auto project = Project().add(target0).create();