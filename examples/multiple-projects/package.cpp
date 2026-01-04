#include <CXPM/Models/Project.hpp>
#include <CXPM/Models/Target.hpp>

using namespace CXPM::Models;

auto project = Project()
    .add("executable")
    .add("library-00")
    .add("library-01")
    .add("library-base")
    .create();