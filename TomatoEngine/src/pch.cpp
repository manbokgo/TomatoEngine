#include "pch.h"

size_t g_AllocationSize = 0;

namespace tomato::Allocation
{
    size_t GetSize() { return g_AllocationSize; }
}

#pragma warning(push)
#pragma warning(disable : 28251)
void* operator new(size_t size)
{
    if (size == 0)
    {
        ++size;
    }

    g_AllocationSize += size;
    if (void* ptr = std::malloc(size))
    {
        return ptr;
    }

    throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void __CRTDECL operator delete(void* ptr, size_t size) noexcept
{
    g_AllocationSize -= size;
    std::free(ptr);
}
#pragma warning(pop)