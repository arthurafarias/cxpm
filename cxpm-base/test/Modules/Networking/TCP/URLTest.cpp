#include "Modules/Networking/HTTP/URL.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Testing/TestCase.hpp"
#include "Modules/Testing/TestGroup.hpp"
#include <memory>

using namespace Modules::Testing;

struct BasicStructureVariantionsTest : public TestCase {
public:
  BasicStructureVariantionsTest()
      : TestCase("BasicStructureVariationsTest", "") {}
  virtual void run() override {

    // scheme://authority/path?query#fragment
    {
      auto source = "scheme://authority/path?query#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme://authority/path?query
    {
      auto source = "scheme://authority/path?query";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme://authority/path#fragment
    {
      auto source = "scheme://authority/path#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme://authority/path
    {
      auto source = "scheme://authority/path";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme://authority
    {
      auto source = "scheme://authority";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "", R"(url.path == "")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/path?query#fragment
    {
      auto source = "scheme:/path?query#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/?query#fragment
    {
      auto source = "scheme:/?query#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/", R"(url.path == "/")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/#fragment
    {
      auto source = "scheme:/#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/", R"(url.path == "/")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/path?query
    {
      auto source = "scheme:/path?query";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/path#fragment
    {
      auto source = "scheme:/path#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/path
    {
      auto source = "scheme:/path";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:
    {
      auto source = "scheme:";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "", R"(url.path == "")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }
  }
};

struct UrlEncodedComponentsTest : public TestCase {
public:
  UrlEncodedComponentsTest() : TestCase("UrlEncodedComponentsTest", "") {}
  virtual void run() override {
    // Encoded space, unicode, reserved chars in path/query/fragment
    {
      auto source =
          "http://example.com/hello%20world/%E2%9C%93?q=a%2Bb#frag%20ment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "http", R"(url.scheme == "http")");
      assert(url.authority == "example.com",
             R"(url.authority == "example.com")");
      assert(url.path == "/hello%20world/%E2%9C%93",
             R"(url.path == "/hello%20world/%E2%9C%93")");
      assert(url.query == "q=a%2Bb", R"(url.query == "q=a%2Bb")");
      assert(url.fragment == "frag%20ment", R"(url.fragment == "frag%20ment")");
    }

    // Encoded authority (rare but legal)
    {
      auto source = "http://user%3Aname:pass%21word@host.com/";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "http", R"(url.scheme == "http")");
      assert(url.authority == "user%3Aname:pass%21word@host.com",
             R"(url.authority == "user%3Aname:pass%21word@host.com")");
      assert(url.path == "/", R"(url.path == "/")");
    }
  }
};

struct AuthorityVariations : public TestCase {
public:
  AuthorityVariations() : TestCase("AuthorityVariations", "") {}
  virtual void run() override {
    // userinfo + host + port
    {
      auto source = "http://user:pass@example.com:8080/path";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "http", R"(url.scheme == "http")");
      assert(url.authority == "user:pass@example.com:8080",
             R"(url.authority == "user:pass@example.com:8080")");
      assert(url.path == "/path", R"(url.path == "/path")");
    }

    // username only
    {
      auto source = "ftp://user@example.com";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.authority == "user@example.com",
             R"(url.authority == "user@example.com")");
    }

    // IPv4 address
    {
      auto source = "http://127.0.0.1:3000/";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.authority == "127.0.0.1:3000",
             R"(url.authority == "127.0.0.1:3000")");
      assert(url.path == "/", R"(url.path == "/")");
    }

    // IPv6 literal
    {
      auto source = "http://[2001:db8::1]/index";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.authority == "[2001:db8::1]",
             R"(url.authority == "[2001:db8::1]")");
      assert(url.path == "/index", R"(url.path == "/index")");
    }

    // IPv6 + port
    {
      auto source = "https://[2001:db8::1]:443/path";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.authority == "[2001:db8::1]:443",
             R"(url.authority == "[2001:db8::1]:443")");
    }
  }
};

struct PathCornerCases : public TestCase {
public:
  PathCornerCases() : TestCase("PathCornerCases", "") {}
  virtual void run() override {
    // Empty path with authority
    {
      auto source = "http://example.com";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.path == "", R"(url.path == "")");
    }

    // Root path
    {
      auto source = "http://example.com/";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.path == "/", R"(url.path == "/")");
    }

    // No leading slash (relative path after scheme:)
    {
      auto source = "scheme:relative/path";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.path == "relative/path", R"(url.path == "relative/path")");
    }
  }
};

struct QueryCornerCases : public TestCase {
public:
  QueryCornerCases() : TestCase("QueryCornerCases", "") {}
  virtual void run() override {
    // scheme://authority/path?query#fragment
    {
      auto source = "scheme://authority/path?query#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme://authority/path?query
    {
      auto source = "scheme://authority/path?query";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme://authority/path#fragment
    {
      auto source = "scheme://authority/path#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme://authority/path
    {
      auto source = "scheme://authority/path";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme://authority
    {
      auto source = "scheme://authority";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "authority", R"(url.authority == "authority")");
      assert(url.path == "", R"(url.path == "")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/path?query#fragment
    {
      auto source = "scheme:/path?query#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/?query#fragment
    {
      auto source = "scheme:/?query#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/", R"(url.path == "/")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/#fragment
    {
      auto source = "scheme:/#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/", R"(url.path == "/")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/path?query
    {
      auto source = "scheme:/path?query";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "query", R"(url.query == "query")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/path#fragment
    {
      auto source = "scheme:/path#fragment";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:/path
    {
      auto source = "scheme:/path";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "/path", R"(url.path == "/path")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // scheme:
    {
      auto source = "scheme:";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.authority == "", R"(url.authority == "")");
      assert(url.path == "", R"(url.path == "")");
      assert(url.query == "", R"(url.query == "")");
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }
  }
};

struct FragmentEdgeCases : public TestCase {
public:
  FragmentEdgeCases() : TestCase("FragmentEdgeCases", "") {}
  virtual void run() override {
    // Empty fragment
    {
      auto source = "http://host/path#";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.fragment == "", R"(url.fragment == "")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    // Non-empty
    {
      auto source = "http://host/path#frag";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.fragment == "frag", R"(url.fragment == "frag")");
      assert(url.string() == source, R"(url.string() == source)");
    }
  }
};

struct MinimalRFCValid : public TestCase {
public:
  MinimalRFCValid() : TestCase("MinimalRFCValid", "") {}
  virtual void run() override {
    {
      auto source = "scheme:";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
      assert(url.string() == source, R"(url.string() == source)");
    }

    {
      auto source = "mailto:user@example.com";
      auto url = Modules::Networking::HTTP::URL::parse(source);
      assert(url.scheme == "mailto", R"(url.scheme == "mailto")");
      assert(url.path == "user@example.com",
             R"(url.path == "user@example.com")");
      assert(url.string() == source, R"(url.string() == source)");
    }
  }
};

class UrlTest : public TestGroup {
public:
  UrlTest() : TestGroup("UrlTest", "") {
    tests = {SharedPointer<TestCase>(new BasicStructureVariantionsTest()),
             SharedPointer<TestCase>(new UrlEncodedComponentsTest()),
             SharedPointer<TestCase>(new MinimalRFCValid()),
             SharedPointer<TestCase>(new QueryCornerCases()),
             SharedPointer<TestCase>(new PathCornerCases())};
  }
};

int main(int argc, char *argv[]) {
  UNUSED(argc);
  UNUSED(argv);
  Logging::LoggerManager::level_set(Logging::LoggerManager::Level::Max);
  Logging::LoggerManager::stream_set(Logging::LoggerManager::stream_cout());
  UrlTest().run();
}
