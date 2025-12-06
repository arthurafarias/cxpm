#pragma once

#include "Core/Containers/Map.hpp"
#include <Core/Containers/String.hpp>

using namespace Core::Containers;

namespace Modules::Networking::HTTP {
struct ResponseStatus {
  int code;
  String description;
};

inline static Map<int, ResponseStatus> ResponseStatuses = {
    {200, ResponseStatus{200, "OK"}}};
} // namespace Modules::Networking::HTTP