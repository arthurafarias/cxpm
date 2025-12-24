#pragma once

#include <Utils/Macros/DisableCopy.hpp>
#include <Utils/Macros/DisableEmptyConstructor.hpp>
#include <Utils/Macros/DisableMove.hpp>

#define StaticClass(ClassName)                                                 \
  DisableEmptyConstructor(ClassName) DisableCopy(ClassName)                    \
      DisableMove(ClassName)
