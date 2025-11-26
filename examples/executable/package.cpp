#include <Project.hpp>
#include <Package.hpp>
#include <BuildManager.hpp>

Package descriptor = {
    .name = "example-executable",
    .version = "1.0.0",
    .sources = {"src/main.cpp"}
};

Project project = {
    .targets = {
        descriptor
    }
};