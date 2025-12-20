#include "Modules/Networking/HTTP/Request.hpp"
#include "Core/Logging/Manager.hpp"
#include "Core/SharedPointer.hpp"
#include "Modules/Testing/TestCase.hpp"
#include "Modules/Testing/TestGroup.hpp"

using namespace Modules::Testing;

struct SimpleParseTest : public TestCase {
  SimpleParseTest() : TestCase("SimpleParseTest", "") {}
  virtual void run() override {

    auto text = R"(
POST / HTTP/1.1
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
  RequestTest() : TestGroup("RequestTest", "") {
    tests = {SharedPointer<TestCase>(new SimpleParseTest())};
  }
};

int main(int argc, char *argv[]) {
  Logging::Logger::level_set(Logging::Logger::Level::Max);
  Logging::Logger::stream_set(Logging::Logger::stream_cout());
  RequestTest().run();
}
