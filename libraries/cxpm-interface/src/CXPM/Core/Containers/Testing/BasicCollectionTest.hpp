#pragma once

#include "CXPM/Core/Containers/BasicCollection.hpp"
#include "CXPM/Core/Containers/String.hpp"
#include "CXPM/Testing/TestGroup.hpp"

using namespace CXPM::Core::Containers;
using namespace CXPM::Testing;

namespace CXPM::Core::Containers::Testing {

struct BasicCollectionTest : public TestGroup {
  BasicCollectionTest()
      : TestGroup(
            "BasicCollection",
            {
                {"transform maps every element into a new collection of "
                 "the target type",
                 [](TestContext &ctx) {
                   BasicCollection<int> numbers{1, 2, 3};
                   auto strings = numbers.transform<String>(
                       [](const int &value) {
                         return String(std::to_string(value));
                       });
                   ctx.equal(strings.size(), std::size_t{3});
                   ctx.equal(strings[0], String("1"));
                   ctx.equal(strings[1], String("2"));
                   ctx.equal(strings[2], String("3"));
                 }},
                {"transform on an empty collection returns an empty "
                 "collection",
                 [](TestContext &ctx) {
                   BasicCollection<int> numbers;
                   auto strings = numbers.transform<String>(
                       [](const int &value) {
                         return String(std::to_string(value));
                       });
                   ctx.check(strings.empty());
                 }},
                {"BasicCollection behaves like the std::deque it inherits "
                 "from",
                 [](TestContext &ctx) {
                   BasicCollection<int> numbers;
                   numbers.push_back(1);
                   numbers.push_back(2);
                   numbers.push_front(0);
                   ctx.equal(numbers.size(), std::size_t{3});
                   ctx.equal(numbers[0], 0);
                   ctx.equal(numbers[2], 2);
                 }},
            }) {}
};

inline static BasicCollectionTest basic_collection_test_instance;

} // namespace CXPM::Core::Containers::Testing
