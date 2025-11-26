#include <Models/BasicTarget.hpp>

Models::BasicTarget cpkg_build_target = {
    .name = "cpkg-build",
    .version = "1.0.0",
    .type = "executable",
    .dependencies = {"cpkg-base"},
    .include_directories = {"src"},
    .sources = {"src/cpkg-build.cpp"},
};