#pragma once

#include "CXPM/Core/Containers/Collection.hpp"
#include "CXPM/Core/Containers/String.hpp"

using namespace CXPM::Core::Containers;

namespace CXPM::Models
{
    class Command : public Collection<String> {
        public:
        using Collection<String>::Collection;
    };
}