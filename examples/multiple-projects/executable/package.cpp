#include <CXPM/Models/Target.hpp>
#include <CXPM/Models/Project.hpp>

using namespace CXPM::Models;

auto example = Target()
                   .name_set("example-executable")
                   .version_set("1.0.0")
                   .language_set("c++")
                   .type_set("executable")
                   .sources_append({"src/main.cpp", "src/source0.cpp",
                                    "src/source1.cpp", "src/source2.cpp"})
                   .sources_append({"src/source3.cpp"})
                   .options_append({"-fPIE", "-fstack-protector-all"})
                   .link_libraries_append({"m", "library-00", "library-01"})
                   .include_directories_append({"src"})
                   .create();

auto project = Project().add(example).create();