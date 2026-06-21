#pragma once

namespace CXPM::Modules::Serialization
{
    struct ArrayEndToken {
        ArrayEndToken(const char* name) : name(name) {}
        const char* name_get() { return name; }
        private:
        const char* name;
    };
}