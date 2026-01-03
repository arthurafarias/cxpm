#include <Models/Project.hpp>
#include <Models/Target.hpp>

auto project = Models::Project()
    .add("executable")
    .add("library-00")
    .add("library-01")
    .add("library-base")
    .create();