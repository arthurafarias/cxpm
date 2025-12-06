#include "Modules/Networking/HTTP/URL.hpp"
#include "Modules/Networking/HTTP/Request.hpp"
#include "Modules/Testing/TestCase.hpp"
#include "Modules/Testing/TestGroup.hpp"
#include <memory>

using namespace Modules::Testing;

struct Test_SchemeAuthorityPathQueryFragment : public TestCase {
public:
  Test_SchemeAuthorityPathQueryFragment()
      : TestCase("Test_SchemeAuthorityPathQueryFragment",
                 "URL = scheme://authority/path?query#fragment") {}

  void run() override {
    const char* source = "scheme://authority/path?query#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "authority", R"(url.authority == "authority")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemeAuthorityPathQuery : public TestCase {
public:
  Test_SchemeAuthorityPathQuery()
      : TestCase("Test_SchemeAuthorityPathQuery",
                 "URL = scheme://authority/path?query") {}

  void run() override {
    const char* source = "scheme://authority/path?query";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "authority", R"(url.authority == "authority")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};


struct Test_SchemeAuthorityPathFragment : public TestCase {
public:
  Test_SchemeAuthorityPathFragment()
      : TestCase("Test_SchemeAuthorityPathFragment",
                 "URL = scheme://authority/path#fragment") {}

  void run() override {
    const char* source = "scheme://authority/path#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "authority", R"(url.authority == "authority")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemeAuthorityPath : public TestCase {
public:
  Test_SchemeAuthorityPath()
      : TestCase("Test_SchemeAuthorityPath",
                 "URL = scheme://authority/path") {}

  void run() override {
    const char* source = "scheme://authority/path";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "authority", R"(url.authority == "authority")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemeAuthority : public TestCase {
public:
  Test_SchemeAuthority()
      : TestCase("Test_SchemeAuthority",
                 "URL = scheme://authority") {}

  void run() override {
    const char* source = "scheme://authority";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "authority", R"(url.authority == "authority")");
    assert(url.path == "", R"(url.path == "")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemePathQueryFragment : public TestCase {
public:
  Test_SchemePathQueryFragment()
      : TestCase("Test_SchemePathQueryFragment",
                 "URL = scheme:/path?query#fragment") {}

  void run() override {
    const char* source = "scheme:/path?query#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};


struct Test_SchemeSlashQueryFragment : public TestCase {
public:
  Test_SchemeSlashQueryFragment()
      : TestCase("Test_SchemeSlashQueryFragment",
                 "URL = scheme:/?query#fragment") {}

  void run() override {
    const char* source = "scheme:/?query#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "/", R"(url.path == "/")");
    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemeSlashFragment : public TestCase {
public:
  Test_SchemeSlashFragment()
      : TestCase("Test_SchemeSlashFragment",
                 "URL = scheme:/#fragment") {}

  void run() override {
    const char* source = "scheme:/#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "/", R"(url.path == "/")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemePathQuery : public TestCase {
public:
  Test_SchemePathQuery()
      : TestCase("Test_SchemePathQuery",
                 "URL = scheme:/path?query") {}

  void run() override {
    const char* source = "scheme:/path?query";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemePathFragment : public TestCase {
public:
  Test_SchemePathFragment()
      : TestCase("Test_SchemePathFragment",
                 "URL = scheme:/path#fragment") {}

  void run() override {
    const char* source = "scheme:/path#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemePath : public TestCase {
public:
  Test_SchemePath()
      : TestCase("Test_SchemePath",
                 "URL = scheme:/path") {}

  void run() override {
    const char* source = "scheme:/path";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemeEmpty : public TestCase {
public:
  Test_SchemeEmpty()
      : TestCase("Test_SchemeEmpty",
                 "URL = scheme:") {}

  void run() override {
    const char* source = "scheme:";
    auto url = Modules::Networking::HTTP::URL::parse(source);
    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "", R"(url.path == "")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_EncodedPathQueryFragment : public TestCase {
public:
  Test_EncodedPathQueryFragment()
      : TestCase("Test_EncodedPathQueryFragment",
                 "URL = http://example.com/hello%20world/%E2%9C%93?q=a%2Bb#frag%20ment") {}

  void run() override {
    const char* source =
        "http://example.com/hello%20world/%E2%9C%93?q=a%2Bb#frag%20ment";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.authority == "example.com", R"(url.authority == "example.com")");
    assert(url.path == "/hello%20world/%E2%9C%93", R"(url.path == "/hello%20world/%E2%9C%93")");
    assert(url.query == "q=a%2Bb", R"(url.query == "q=a%2Bb")");
    assert(url.fragment == "frag%20ment", R"(url.fragment == "frag%20ment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_EncodedAuthority : public TestCase {
public:
  Test_EncodedAuthority()
      : TestCase("Test_EncodedAuthority",
                 "URL = http://user%3Aname:pass%21word@host.com/") {}

  void run() override {
    const char* source = "http://user%3Aname:pass%21word@host.com/";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.authority == "user%3Aname:pass%21word@host.com", R"(url.authority == "user%3Aname:pass%21word@host.com")");
    assert(url.path == "/", R"(url.path == "/")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_UserPassHostPortPath : public TestCase {
public:
  Test_UserPassHostPortPath()
      : TestCase("Test_UserPassHostPortPath",
                 "URL = http://user:pass@example.com:8080/path") {}

  void run() override {
    const char* source = "http://user:pass@example.com:8080/path";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.authority == "user:pass@example.com:8080", R"(url.authority == "user:pass@example.com:8080")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_UsernameOnlyAuthority : public TestCase {
public:
  Test_UsernameOnlyAuthority()
      : TestCase("Test_UsernameOnlyAuthority",
                 "URL = ftp://user@example.com") {}

  void run() override {
    const char* source = "ftp://user@example.com";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.authority == "user@example.com", R"(url.authority == "user@example.com")");
    assert(url.scheme == "ftp", R"(url.scheme == "ftp")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_IPv4WithPortAndRootPath : public TestCase {
public:
  Test_IPv4WithPortAndRootPath()
      : TestCase("Test_IPv4WithPortAndRootPath",
                 "URL = http://127.0.0.1:3000/") {}

  void run() override {
    const char* source = "http://127.0.0.1:3000/";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.authority == "127.0.0.1:3000", R"(url.authority == "127.0.0.1:3000")");
    assert(url.path == "/", R"(url.path == "/")");
    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_IPv6Literal : public TestCase {
public:
  Test_IPv6Literal()
      : TestCase("Test_IPv6Literal",
                 "URL = http://[2001:db8::1]/index") {}

  void run() override {
    const char* source = "http://[2001:db8::1]/index";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.authority == "[2001:db8::1]", R"(url.authority == "[2001:db8::1]")");
    assert(url.path == "/index", R"(url.path == "/index")");
    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_IPv6WithPort : public TestCase {
public:
  Test_IPv6WithPort()
      : TestCase("Test_IPv6WithPort",
                 "URL = https://[2001:db8::1]:443/path") {}

  void run() override {
    const char* source = "https://[2001:db8::1]:443/path";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.authority == "[2001:db8::1]:443", R"(url.authority == "[2001:db8::1]:443")");
    assert(url.scheme == "https", R"(url.scheme == "https")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_EmptyPathAfterAuthority : public TestCase {
public:
  Test_EmptyPathAfterAuthority()
      : TestCase("Test_EmptyPathAfterAuthority",
                 "URL = http://example.com") {}

  void run() override {
    const char* source = "http://example.com";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.authority == "example.com", R"(url.authority == "example.com")");
    assert(url.path == "", R"(url.path == "")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};


struct Test_RootPathOnly : public TestCase {
public:
  Test_RootPathOnly()
      : TestCase("Test_RootPathOnly",
                 "URL = http://example.com/") {}

  void run() override {
    const char* source = "http://example.com/";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.authority == "example.com", R"(url.authority == "example.com")");
    assert(url.path == "/", R"(url.path == "/")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_MultiSegmentPath : public TestCase {
public:
  Test_MultiSegmentPath()
      : TestCase("Test_MultiSegmentPath",
                 "URL = http://example.com/a/b/c") {}

  void run() override {
    const char* source = "http://example.com/a/b/c";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.path == "/a/b/c", R"(url.path == "/a/b/c")");
    assert(url.authority == "example.com", R"(url.authority == "example.com")");
    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_PathWithDotSegments : public TestCase {
public:
  Test_PathWithDotSegments()
      : TestCase("Test_PathWithDotSegments",
                 "URL = http://example.com/a/./b/../c") {}

  void run() override {
    const char* source = "http://example.com/a/./b/../c";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    // IMPORTANT: We assume your URL class preserves raw path (no normalization).
    assert(url.path == "/a/./b/../c", R"(url.path == "/a/./b/../c")");
    assert(url.authority == "example.com", R"(url.authority == "example.com")");
    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_PathWithEncodedSlash : public TestCase {
public:
  Test_PathWithEncodedSlash()
      : TestCase("Test_PathWithEncodedSlash",
                 "URL = http://example.com/a%2Fb/c") {}

  void run() override {
    const char* source = "http://example.com/a%2Fb/c";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.path == "/a%2Fb/c", R"(url.path == "/a%2Fb/c")");
    assert(url.authority == "example.com", R"(url.authority == "example.com")");
    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_PathWithUnicode : public TestCase {
public:
  Test_PathWithUnicode()
      : TestCase("Test_PathWithUnicode",
                 "URL = http://example.com/✓/路径") {}

  void run() override {
    const char* source = "http://example.com/✓/路径";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.path == "/✓/路径", R"(url.path == "/✓/路径")");
    assert(url.authority == "example.com", R"(url.authority == "example.com")");
    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_QueryAndFragment : public TestCase {
public:
  Test_QueryAndFragment()
      : TestCase("Test_QueryAndFragment",
                 "URL = scheme://authority/path?query#fragment") {}

  void run() override {
    const char* source = "scheme://authority/path?query#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "authority", R"(url.authority == "authority")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_QueryOnly : public TestCase {
public:
  Test_QueryOnly()
      : TestCase("Test_QueryOnly",
                 "URL = scheme://authority/path?query") {}

  void run() override {
    const char* source = "scheme://authority/path?query";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_FragmentOnlyUrl : public TestCase {
public:
  Test_FragmentOnlyUrl()
      : TestCase("Test_FragmentOnlyUrl",
                 "URL = scheme://authority/path#fragment") {}

  void run() override {
    const char* source = "scheme://authority/path#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_NoQueryNoFragment : public TestCase {
public:
  Test_NoQueryNoFragment()
      : TestCase("Test_NoQueryNoFragment",
                 "URL = scheme://authority/path") {}

  void run() override {
    const char* source = "scheme://authority/path";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_Query_EmptyAuthority : public TestCase {
public:
  Test_Query_EmptyAuthority()
      : TestCase("Test_Query_EmptyAuthority",
                 "URL = scheme:/path?query#fragment") {}

  void run() override {
    const char* source = "scheme:/path?query#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.authority == "", R"(url.authority == "")");
    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_QueryAtRoot : public TestCase {
public:
  Test_QueryAtRoot()
      : TestCase("Test_QueryAtRoot",
                 "URL = scheme:/?query#fragment") {}

  void run() override {
    const char* source = "scheme:/?query#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.path == "/", R"(url.path == "/")");
    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_FragmentAtRoot : public TestCase {
public:
  Test_FragmentAtRoot()
      : TestCase("Test_FragmentAtRoot",
                 "URL = scheme:/#fragment") {}

  void run() override {
    const char* source = "scheme:/#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.path == "/", R"(url.path == "/")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_QueryWithoutFragment_EmptyAuthority : public TestCase {
public:
  Test_QueryWithoutFragment_EmptyAuthority()
      : TestCase("Test_QueryWithoutFragment_EmptyAuthority",
                 "URL = scheme:/path?query") {}

  void run() override {
    const char* source = "scheme:/path?query";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.query == "query", R"(url.query == "query")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_FragmentWithoutQuery_EmptyAuthority : public TestCase {
public:
  Test_FragmentWithoutQuery_EmptyAuthority()
      : TestCase("Test_FragmentWithoutQuery_EmptyAuthority",
                 "URL = scheme:/path#fragment") {}

  void run() override {
    const char* source = "scheme:/path#fragment";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.fragment == "fragment", R"(url.fragment == "fragment")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemeColonOnlyAgain : public TestCase {
public:
  Test_SchemeColonOnlyAgain()
      : TestCase("Test_SchemeColonOnlyAgain",
                 "URL = scheme:") {}

  void run() override {
    const char* source = "scheme:";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "", R"(url.path == "")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_EmptyFragment : public TestCase {
public:
  Test_EmptyFragment()
      : TestCase("Test_EmptyFragment",
                 "URL = http://host/path#") {}

  void run() override {
    const char* source = "http://host/path#";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.authority == "host", R"(url.authority == "host")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.fragment == "", R"(url.frament == "")");
    // assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_NonEmptyFragment : public TestCase {
public:
  Test_NonEmptyFragment()
      : TestCase("Test_NonEmptyFragment",
                 "URL = http://host/path#frag") {}

  void run() override {
    const char* source = "http://host/path#frag";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.fragment == "frag", R"(url.fragment == "frag")");
    assert(url.scheme == "http", R"(url.scheme == "http")");
    assert(url.authority == "host", R"(url.authority == "host")");
    assert(url.path == "/path", R"(url.path == "/path")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_SchemeOnly : public TestCase {
public:
  Test_SchemeOnly()
      : TestCase("Test_SchemeOnly",
                 "URL = scheme:") {}

  void run() override {
    const char* source = "scheme:";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "scheme", R"(url.scheme == "scheme")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.path == "", R"(url.path == "")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};

struct Test_MailtoScheme : public TestCase {
public:
  Test_MailtoScheme()
      : TestCase("Test_MailtoScheme",
                 "URL = mailto:user@example.com") {}

  void run() override {
    const char* source = "mailto:user@example.com";
    auto url = Modules::Networking::HTTP::URL::parse(source);

    assert(url.scheme == "mailto", R"(url.scheme == "mailto")");
    assert(url.path == "user@example.com", R"(url.path == "user@example.com")");
    assert(url.authority == "", R"(url.authority == "")");
    assert(url.query == "", R"(url.query == "")");
    assert(url.fragment == "", R"(url.fragment == "")");
    assert(url.string() == source, R"(url.string() == source)");
  }
};


class UrlTest : public TestGroup {
public:
  UrlTest() : TestGroup("UrlTest", "") {
    tests = {
        // Basic structure variations
        SharedPointer<TestCase>(new Test_SchemeAuthorityPathQueryFragment()),
        SharedPointer<TestCase>(new Test_SchemeAuthorityPathQuery()),
        SharedPointer<TestCase>(new Test_SchemeAuthorityPathFragment()),
        SharedPointer<TestCase>(new Test_SchemeAuthorityPath()),
        // SharedPointer<TestCase>(new Test_SchemeAuthorityOnly()),
        SharedPointer<TestCase>(new Test_SchemePathQueryFragment()),
        // SharedPointer<TestCase>(new Test_SchemeRootQueryFragment()),
        // SharedPointer<TestCase>(new Test_SchemeRootFragment()),
        SharedPointer<TestCase>(new Test_SchemePathQuery()),
        SharedPointer<TestCase>(new Test_SchemePathFragment()),
        SharedPointer<TestCase>(new Test_SchemePath()),
        SharedPointer<TestCase>(new Test_SchemeColonOnlyAgain()),

        // Fragment edge cases
        SharedPointer<TestCase>(new Test_EmptyFragment()),
        SharedPointer<TestCase>(new Test_NonEmptyFragment()),

        // Minimal RFC valid URLs
        SharedPointer<TestCase>(new Test_SchemeOnly()),
        SharedPointer<TestCase>(new Test_MailtoScheme()),

        // URL encoded components
        // SharedPointer<TestCase>(new Test_UrlEncodedPathQueryFragment()),
        // SharedPointer<TestCase>(new Test_UrlEncodedAuthority()),

        // Authority variations
        // SharedPointer<TestCase>(new Test_AuthorityUserPassPort()),
        // SharedPointer<TestCase>(new Test_AuthorityUsernameOnly()),
        // SharedPointer<TestCase>(new Test_AuthorityIPv4()),
        // SharedPointer<TestCase>(new Test_AuthorityIPv6Literal()),
        // SharedPointer<TestCase>(new Test_AuthorityIPv6WithPort()),

        // Path corner cases
        // SharedPointer<TestCase>(new Test_PathEmptyWithAuthority()),
        // SharedPointer<TestCase>(new Test_PathRoot()),
        // SharedPointer<TestCase>(new Test_PathRelativeNoSlash()),

        // Query corner cases
        SharedPointer<TestCase>(new Test_QueryAndFragment()),
        SharedPointer<TestCase>(new Test_QueryOnly()),
        SharedPointer<TestCase>(new Test_FragmentOnlyUrl()),
        SharedPointer<TestCase>(new Test_NoQueryNoFragment()),
        SharedPointer<TestCase>(new Test_Query_EmptyAuthority()),
        SharedPointer<TestCase>(new Test_QueryAtRoot()),
        SharedPointer<TestCase>(new Test_FragmentAtRoot()),
        SharedPointer<TestCase>(new Test_QueryWithoutFragment_EmptyAuthority()),
        SharedPointer<TestCase>(new Test_FragmentWithoutQuery_EmptyAuthority())
    };
  }
};


int main(int argc, char *argv[]) {
  UNUSED(argc);
  UNUSED(argv);
  Logging::LoggerManager::level_set(Logging::LoggerManager::Level::Max);
  Logging::LoggerManager::stream_set(Logging::LoggerManager::stream_cout());
  UrlTest().run();
}
