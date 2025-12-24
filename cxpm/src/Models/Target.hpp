#pragma once

#include "Models/TargetFactoryInterfaceDescriptor.hpp"
#include <Models/TargetDescriptor.hpp>

namespace Models {
struct Target : public TargetDescriptorInterface<Target> {
  using TargetDescriptorInterface<Target>::TargetDescriptorInterface;
};
} // namespace Models