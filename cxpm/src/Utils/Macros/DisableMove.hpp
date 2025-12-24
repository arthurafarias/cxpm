#pragma once

#define DisableMove(ClassName) \
    ClassName(const ClassName&&) = delete; \
    ClassName& operator=(const ClassName&&) = delete;


    