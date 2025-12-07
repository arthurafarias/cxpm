#pragma once

#include <ostream>
#include <sstream>

class OutputStringStream : public std::ostringstream
{
    public:
    using std::ostringstream::basic_ostream;
};