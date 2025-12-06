#include "Modules/Networking/HTTP/URL.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Testing/TestCase.hpp"
#include "Modules/Testing/TestGroup.hpp"
#include <cassert>
#include <memory>

using namespace Modules::Testing;

struct BasicStructureVariantionsTest : public TestCase {
public:
  BasicStructureVariantionsTest()
      : TestCase("BasicStructureVariationsTest", "") {}
  virtual void run() override {

    // scheme://authority/path?query#fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "scheme://authority/path?query#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "/path");
      assert(url.query == "query");
      assert(url.fragment == "fragment");
    }

    // scheme://authority/path?query
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "scheme://authority/path?query");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "/path");
      assert(url.query == "query");
      assert(url.fragment == "");
    }

    // scheme://authority/path#fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "scheme://authority/path#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "/path");
      assert(url.query == "");
      assert(url.fragment == "fragment");
    }

    // scheme://authority/path
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("scheme://authority/path");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "/path");
      assert(url.query == "");
      assert(url.fragment == "");
    }

    // scheme://authority
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme://authority");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "");
      assert(url.query == "");
      assert(url.fragment == "");
    }

    // scheme:/path?query#fragment
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("scheme:/path?query#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/path");
      assert(url.query == "query");
      assert(url.fragment == "fragment");
    }

    // scheme:/?query#fragment
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("scheme:/?query#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/");
      assert(url.query == "query");
      assert(url.fragment == "fragment");
    }

    // scheme:/#fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:/#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/");
      assert(url.query == "");
      assert(url.fragment == "fragment");
    }

    // scheme:/path?query
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:/path?query");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/path");
      assert(url.query == "query");
      assert(url.fragment == "");
    }

    // scheme:/path#fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:/path#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/path");
      assert(url.query == "");
      assert(url.fragment == "fragment");
    }

    // scheme:/path
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:/path");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/path");
      assert(url.query == "");
      assert(url.fragment == "");
    }

    // scheme:
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "");
      assert(url.query == "");
      assert(url.fragment == "");
    }
  }
};

struct UrlEncodedComponentsTest : public TestCase {
public:
  UrlEncodedComponentsTest() : TestCase("UrlEncodedComponentsTest", "") {}
  virtual void run() override {
    // Encoded space, unicode, reserved chars in path/query/fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "http://example.com/hello%20world/%E2%9C%93?q=a%2Bb#frag%20ment");
      assert(url.scheme == "http");
      assert(url.authority == "example.com");
      assert(url.path == "/hello%20world/%E2%9C%93");
      assert(url.query == "q=a%2Bb");
      assert(url.fragment == "frag%20ment");
    }

    // Encoded authority (rare but legal)
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "http://user%3Aname:pass%21word@host.com/");
      assert(url.scheme == "http");
      assert(url.authority == "user%3Aname:pass%21word@host.com");
      assert(url.path == "/");
    }
  }
};

struct AuthorityVariations : public TestCase {
public:
  AuthorityVariations() : TestCase("AuthorityVariations", "") {}
  virtual void run() override {
    // userinfo + host + port
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "http://user:pass@example.com:8080/path");
      assert(url.scheme == "http");
      assert(url.authority == "user:pass@example.com:8080");
      assert(url.path == "/path");
    }

    // username only
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("ftp://user@example.com");
      assert(url.authority == "user@example.com");
    }

    // IPv4 address
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("http://127.0.0.1:3000/");
      assert(url.authority == "127.0.0.1:3000");
      assert(url.path == "/");
    }

    // IPv6 literal
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("http://[2001:db8::1]/index");
      assert(url.authority == "[2001:db8::1]");
      assert(url.path == "/index");
    }

    // IPv6 + port
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "https://[2001:db8::1]:443/path");
      assert(url.authority == "[2001:db8::1]:443");
    }
  }
};

struct PathCornerCases : public TestCase {
public:
  PathCornerCases() : TestCase("PathCornerCases", "") {}
  virtual void run() override {
    // Empty path with authority
    {
      auto url = Modules::Networking::HTTP::URL::parse("http://example.com");
      assert(url.path == "");
    }

    // Root path
    {
      auto url = Modules::Networking::HTTP::URL::parse("http://example.com/");
      assert(url.path == "/");
    }

    // No leading slash (relative path after scheme:)
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:relative/path");
      assert(url.path == "relative/path");
    }
  }
};

struct QueryCornerCases : public TestCase {
public:
  QueryCornerCases() : TestCase("QueryCornerCases", "") {}
  virtual void run() override {
    // scheme://authority/path?query#fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "scheme://authority/path?query#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "/path");
      assert(url.query == "query");
      assert(url.fragment == "fragment");
    }

    // scheme://authority/path?query
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "scheme://authority/path?query");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "/path");
      assert(url.query == "query");
      assert(url.fragment == "");
    }

    // scheme://authority/path#fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse(
          "scheme://authority/path#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "/path");
      assert(url.query == "");
      assert(url.fragment == "fragment");
    }

    // scheme://authority/path
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("scheme://authority/path");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "/path");
      assert(url.query == "");
      assert(url.fragment == "");
    }

    // scheme://authority
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme://authority");
      assert(url.scheme == "scheme");
      assert(url.authority == "authority");
      assert(url.path == "");
      assert(url.query == "");
      assert(url.fragment == "");
    }

    // scheme:/path?query#fragment
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("scheme:/path?query#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/path");
      assert(url.query == "query");
      assert(url.fragment == "fragment");
    }

    // scheme:/?query#fragment
    {
      auto url =
          Modules::Networking::HTTP::URL::parse("scheme:/?query#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/");
      assert(url.query == "query");
      assert(url.fragment == "fragment");
    }

    // scheme:/#fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:/#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/");
      assert(url.query == "");
      assert(url.fragment == "fragment");
    }

    // scheme:/path?query
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:/path?query");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/path");
      assert(url.query == "query");
      assert(url.fragment == "");
    }

    // scheme:/path#fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:/path#fragment");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/path");
      assert(url.query == "");
      assert(url.fragment == "fragment");
    }

    // scheme:/path
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:/path");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "/path");
      assert(url.query == "");
      assert(url.fragment == "");
    }

    // scheme:
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:");
      assert(url.scheme == "scheme");
      assert(url.authority == "");
      assert(url.path == "");
      assert(url.query == "");
      assert(url.fragment == "");
    }
  }
};

struct FragmentEdgeCases : public TestCase {
public:
  FragmentEdgeCases() : TestCase("FragmentEdgeCases", "") {}
  virtual void run() override {
    // Empty fragment
    {
      auto url = Modules::Networking::HTTP::URL::parse("http://host/path#");
      assert(url.fragment == "");
    }

    // Non-empty
    {
      auto url = Modules::Networking::HTTP::URL::parse("http://host/path#frag");
      assert(url.fragment == "frag");
    }
  }
};

struct MinimalRFCValid : public TestCase {
public:
  MinimalRFCValid() : TestCase("MinimalRFCValid", "") {}
  virtual void run() override {
    {
      auto url = Modules::Networking::HTTP::URL::parse("scheme:");
      assert(url.scheme == "scheme");
    }

    {
      auto url =
          Modules::Networking::HTTP::URL::parse("mailto:user@example.com");
      assert(url.scheme == "mailto");
      assert(url.path == "user@example.com");
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
