#pragma once

namespace CXPM::Modules::Serialization
{
    struct ObjectEndToken {
        ObjectEndToken(const char* name) : name(name) {}
        const char* name_get() { return name; }
        private:
        const char* name;
    };
}