
#include <Compiler.hpp>
#include <Package.hpp>
#include <Toolchain.hpp>
#include <Variant.hpp>
#include <Workspace.hpp>
#include <Project.hpp>

extern Project project;

extern "C" const Project* get_project()  { return &project; }

