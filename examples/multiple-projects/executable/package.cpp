#include <CXPM/Models/Target.hpp>
#include <CXPM/Models/Project.hpp>

auto example = Models::Target()
                   .name_set("example-executable")
                   .version_set("1.0.0")
                   .type_set("executable")
                   .sources_append({"src/main.cpp", "src/source0.cpp",
                                    "src/source1.cpp", "src/source2.cpp"})
                   .sources_append({"src/source3.cpp"})
                   .options_append({"-fPIE", "-fstack-protector-all"})
                   .link_libraries_append({"m"})
                   .include_directories_append({"src"})
                   .dependencies_append("library-00")
                   .dependencies_append("library-01")
                   .create();

auto project = Models::Project().add(example).create();