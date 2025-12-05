#pragma once

#include <CXPM/Utils/Macros/DisableCopy.hpp>
#include <CXPM/Utils/Macros/DisableEmptyConstructor.hpp>
#include <CXPM/Utils/Macros/DisableMove.hpp>

#define StaticClass(ClassName)                                                 \
  DisableEmptyConstructor(ClassName) DisableCopy(ClassName)                    \
      DisableMove(ClassName)
