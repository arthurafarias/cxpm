#include <Package.hpp>

Package package = {
    .name = "Test",
    .version = "1.0",
    .sources = { "src/main.cpp" },
    .options = { "-fPIC" },
    .link_libraries = { "m" }
};