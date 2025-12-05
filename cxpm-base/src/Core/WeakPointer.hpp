#pragma once

#include <memory>

template<typename ...ArgsTypes>
class WeakPointer : public std::weak_ptr<ArgsTypes...>
{
    public:
    using std::weak_ptr<ArgsTypes...>::weak_ptr;
};