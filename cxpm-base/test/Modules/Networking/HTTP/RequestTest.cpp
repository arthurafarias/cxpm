#include "Modules/Networking/HTTP/Request.hpp"
#include "Core/Logging/LoggerManager.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Testing/TestCase.hpp"
#include "Modules/Testing/TestGroup.hpp"

using namespace Modules::Testing;

struct SimpleParseTest : public TestCase {
  SimpleParseTest() : TestCase("SimpleParseTest", "") {}
  virtual void run() override {

    auto text = R"(
GET / HTTP/1.1
Host: api.com
Content-Type: text/json

{
            "data": "Hello World"
}



            )";

    auto request = Modules::Networking::HTTP::Request::parse(text);
  }
};

class RequestTest : public TestGroup {
public:
  RequestTest() : TestGroup("RequestTest", "") { tests = {SharedPointer<TestCase>(new SimpleParseTest())}; }
};

int main(int argc, char *argv[]) {
  Logging::LoggerManager::level_set(Logging::LoggerManager::Level::Max);
  Logging::LoggerManager::stream_set(Logging::LoggerManager::stream_cout());
  RequestTest().run();
}
