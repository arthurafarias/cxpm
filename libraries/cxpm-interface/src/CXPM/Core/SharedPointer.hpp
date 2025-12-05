#pragma once

#include <memory>

template<typename ...ArgsTypes>
class SharedPointer : public std::shared_ptr<ArgsTypes...>
{
    public:
    using std::shared_ptr<ArgsTypes...>::shared_ptr;
};