#pragma once

#include <CXPM/Models/BuildOutputResult.hpp>
#include <CXPM/Core/Containers/String.hpp>

#include <future>
#include <memory>
#include <tuple>

using namespace CXPM::Core::Containers;

namespace CXPM::Models {
struct ToolchainBasicCommandInterface {
  using promise_type = std::shared_ptr<std::promise<
      std::tuple<int, BuildOutputResult>>>;
};
} // namespace CXPM::Models
