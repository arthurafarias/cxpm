#include <Models/BasicProject.hpp>
#include <Models/BasicTarget.hpp>

auto descriptor = Models::BasicTargetFactory::instance()
                      .name("example-executable")
                      .version("1.0.0")
                      .type("executable")
                      .sources({"src/main.cpp", "src/source0.cpp",
                                "src/source1.cpp", "src/source2.cpp"})
                      .sources({"src/source3.cpp"})
                      .options({"-fPIE", "-fstack-protector-all"})
                      .link_libraries({"m"})
                      .create();

// Models::BasicProject project = {.targets = {descriptor}};
auto project = Models::BasicProjectFactory::instance().add(descriptor).create();