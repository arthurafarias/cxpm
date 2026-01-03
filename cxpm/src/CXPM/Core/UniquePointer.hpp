#pragma once

#include <memory>

template<typename ...ArgsTypes>
class UniquePointer : public std::unique_ptr<ArgsTypes...>
{
    public:
    using std::unique_ptr<ArgsTypes...>::unique_ptr;
};