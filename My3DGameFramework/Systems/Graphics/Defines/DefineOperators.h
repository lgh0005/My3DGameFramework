#pragma once

/*=========================================================================//
//     Allocate operator overloadings for static factory class pattern     //
//=========================================================================*/
#define MGF_DECLARE_CUSTOM_ALLOCATORS()                 \
public:                                                 \
    static void* operator new(usize size);              \
    static void operator delete(void* ptr, usize size)

#define MGF_IMPLEMENT_CUSTOM_ALLOCATORS(className)      \
void* className::operator new(usize size)               \
{                                                       \
    return MGF_MEMORY.Allocate(size);                   \
}                                                       \
void className::operator delete(void* ptr, usize size)  \
{                                                       \
    MGF_MEMORY.Deallocate(ptr, size);                   \
}
