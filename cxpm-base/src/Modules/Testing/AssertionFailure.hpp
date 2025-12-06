#pragma once

#include <stdexcept>
namespace Modules::Testing
{
    class AssertionFailure : public std::runtime_error
    {
        public:
        using std::runtime_error::runtime_error;
    };
}