#pragma once

/*=========================================================================//
//     Allocate operator overloadings for static factory class pattern     //
//=========================================================================*/
#define MGF_DECLARE_CUSTOM_ALLOCATORS                                   \
public:                                                                 \
    static void* operator new(usize size) noexcept;                     \
    static void  operator delete(void* ptr, usize size) noexcept;       \
    static void* operator new[](usize size) noexcept;                   \
    static void  operator delete[](void* ptr, usize size) noexcept

#define MGF_IMPLEMENT_CUSTOM_ALLOCATORS(className)                      \
    void* className::operator new(usize size) noexcept                  \
    {                                                                   \
        return MGF_MEMORY.Allocate(size);                               \
    }                                                                   \
    void className::operator delete(void* ptr, usize size) noexcept     \
    {                                                                   \
        MGF_MEMORY.Deallocate(ptr, size);                               \
    }                                                                   \
    void* className::operator new[](usize size) noexcept                \
    {                                                                   \
        return MGF_MEMORY.Allocate(size);                               \
    }                                                                   \
    void className::operator delete[](void* ptr, usize size) noexcept   \
    {                                                                   \
        MGF_MEMORY.Deallocate(ptr, size);                               \
    }