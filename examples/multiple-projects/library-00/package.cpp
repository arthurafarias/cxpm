#include <CXPM/Models/Project.hpp>
#include <CXPM/Models/Target.hpp>

auto target0 = Models::Target()
                   .name_set("library-00")
                   .version_set("1.0.0")
                   .type_set("shared-library")
                   .sources_append({"src/library.cpp"})
                   .options_append({"-fPIC", "-fstack-protector-all"})
                   .link_libraries_append({"m"})
                   .include_directories_append({"src"})
                   .dependencies_append("library-base")
                   .create();

auto project = Models::Project().add(target0).create();