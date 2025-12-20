#include "Modules/Serialization/JSON/JSONArray.hpp"
#include "Modules/Serialization/JSON/JSONObject.hpp"
#include "Modules/Serialization/JSON/JSONOutputArchiver.hpp"
#include "Modules/Serialization/JSON/JSONValue.hpp"
#include "Modules/Streams/OutputStringStream.hpp"
#include <cassert>
#include <cstdlib>

using namespace Modules::Serialization;
using namespace Modules::Serialization::JSON;
using namespace Core::Containers;

int main(int argc, char *argv[]) {

  Modules::Serialization::JSON::JSONObject object;
  object["name"] = JSONArray{
      nullptr, 1, 2.0, "Hi", JSONObject{{"second", "value"}}, JSONArray{}};

  Modules::Streams::OutputStringStream oss;
  JSONOutputArchiver joa(oss);

  joa % object;

  std::cout << oss.str() << std::endl;

  assert(oss.str() == R"({"name":[null,1,2,"Hi",{"second":"value"},[]]})");

  return EXIT_SUCCESS;
}