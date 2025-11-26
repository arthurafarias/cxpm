#include <Models/BasicProject.hpp>
#include <Models/BasicTarget.hpp>

Models::BasicTarget descriptor = {
    .name = "example-executable",
    .version = "1.0.0",
    .type = "executable",
    .sources = {"src/main.cpp", "src/source0.cpp", "src/source1.cpp", "src/source2.cpp", "src/source3.cpp"}
};

Models::BasicProject project = {
    .targets = {
        descriptor
    }
};