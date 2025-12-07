#pragma once

#include <streambuf>

class StreamBuffer : public std::streambuf {
public:
  using std::streambuf::basic_streambuf;
};