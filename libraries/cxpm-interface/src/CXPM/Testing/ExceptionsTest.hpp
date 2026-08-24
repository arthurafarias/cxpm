#pragma once

#include "CXPM/Core/Exceptions/NotImplementedException.hpp"
#include "CXPM/Core/Exceptions/RuntimeException.hpp"
#include "CXPM/Testing/TestGroup.hpp"

#include <string>

namespace CXPM::Testing {

struct ExceptionsTest : public TestGroup {
  ExceptionsTest()
      : TestGroup(
            "Exceptions",
            {
                {"RuntimeException formats its message using std::format "
                 "arguments",
                 [](TestContext &ctx) {
                   CXPM::Core::Exceptions::RuntimeException error(
                       "failed to build {} ({})", "target", 42);
                   ctx.equal(std::string(error.what()),
                             std::string("failed to build target (42)"));
                 }},
                {"RuntimeException is catchable as std::runtime_error",
                 [](TestContext &ctx) {
                   ctx.throws<std::runtime_error>([] {
                     throw CXPM::Core::Exceptions::RuntimeException(
                         "boom");
                   });
                 }},
                {"NotImplementedException defaults to \"Not Implemented!\"",
                 [](TestContext &ctx) {
                   CXPM::Core::Exceptions::NotImplementedException error;
                   ctx.equal(std::string(error.what()),
                             std::string("Not Implemented!"));
                 }},
                {"NotImplementedException is catchable as RuntimeException",
                 [](TestContext &ctx) {
                   ctx.throws<CXPM::Core::Exceptions::RuntimeException>([] {
                     throw CXPM::Core::Exceptions::NotImplementedException();
                   });
                 }},
            }) {}
};

inline static ExceptionsTest exceptions_test_instance;

} // namespace CXPM::Testing
