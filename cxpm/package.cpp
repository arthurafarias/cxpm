#include <CXPM/Models/Target.hpp>
#include <CXPM/Models/Project.hpp>

using namespace CXPM::Models;

auto cxpm = Target()
                    .name_set("cxpm")
                    .version_set("0.1.0")
                    .type_set("executable")
                    .sources_append({"src/cxpm.cpp"})
                    .include_directories_append({"src"})
                    .options_append({"-fPIE", "-fstack-protector-all", "-std=c++23"})
                    .link_libraries_append({"m"})
                    .create();

auto project = Project()
                        .add(cxpm)
                        .create();