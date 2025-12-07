#include "Modules/Networking/HTTP/Response.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Modules/Networking/TCP/Socket.hpp"
#include "Modules/Testing/TestCase.hpp"
#include "Modules/Testing/TestGroup.hpp"

using namespace Modules::Testing;

struct SimpleParseTest : public TestCase {
  SimpleParseTest() : TestCase("SimpleParseTest", "") {}
  virtual void run() override {

    Modules::Networking::HTTP::Response res{
        .descriptor = {.status = {200, "OK"},
                       .version = {1, 1},
                       .headers = {{"Content-Type", "text/json"}},
                       .body = R"({"message": "text"})"},
        .socket = nullptr };

        res.send();
  }
};

class ResponseTest : public TestGroup {
public:
  ResponseTest() : TestGroup("ResponseTest", "") {
    tests = {SharedPointer<TestCase>(new SimpleParseTest())};
  }
};

int main(int argc, char *argv[]) {
  Logging::LoggerManager::level_set(Logging::LoggerManager::Level::Max);
  Logging::LoggerManager::stream_set(Logging::LoggerManager::stream_cout());
  ResponseTest().run();
}
