#pragma once

#include <set>
namespace CXPM::Core::Containers
{
    template<typename ...ArgsTypes>
    class Set : public std::set<ArgsTypes...>
    {
        public:
        using std::set<ArgsTypes...>::set;
    };
}