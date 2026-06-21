#pragma once

#include "CXPM/Modules/Serialization/Token.hpp"

namespace CXPM::Modules::Serialization
{
    struct ObjectStartToken {
        ObjectStartToken(const char* name) : name(name) {}
        const char* name_get() { return name; }
        private:
        const char* name;
    };
}