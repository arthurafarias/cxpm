#pragma once

#include <Models/BasicTarget.hpp>
#include <Models/BasicToolchain.hpp>

#include <deque>

namespace Models {
struct BasicProject {
  const std::deque<std::variant<BasicTarget, BasicToolchain>> targets;
};
} // namespace Models