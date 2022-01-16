#pragma once
#include "AssertMacros.h"

// Safe release of COM pointers
template<typename T>
inline void SafeRelease(T& com_ptr)
{
    CHECK(com_ptr != nullptr);
    com_ptr->Release();
    com_ptr = nullptr;
}
