#pragma once

/*===================================//
//     Default Class Constraints     //
//===================================*/
// Copy constructor & assignment blocking
#define DISABLE_COPY(classType)                                                 \
    classType(const classType&) = delete;                                       \
    classType& operator=(const classType&) = delete;

// Move semantic blocking
#define DISABLE_MOVE(classType)                                                 \
    classType(classType&&) = delete;                                            \
    classType& operator=(classType&&) = delete;

// Static methods only (Utility classes)
#define DECLARE_UTILITIES(classType)                                            \
private:                                                                        \
    classType() = delete;                                                       \
    ~classType() = delete;                                                      \
    DISABLE_COPY(classType)                                                     \
    DISABLE_MOVE(classType)

// Singleton declaration (Meyers' Singleton)
#define DECLARE_SINGLE(classType)                                               \
private:                                                                        \
    DISABLE_COPY(classType)                                                     \
    DISABLE_MOVE(classType)                                                     \
                                                                                \
public:                                                                         \
    static classType& Instance()                                                \
    {                                                                           \
        static classType s_instance;                                            \
        return s_instance;                                                      \
    }

/*================================//
//   Class Pointer Declarations   //
//================================*/
#define CLASS_PTR(className)                                                    \
class className;                                                                \
using className ## UPtr = std::unique_ptr<className>;                           \
using className ## Ptr = std::shared_ptr<className>;                            \
using className ## WPtr = std::weak_ptr<className>;

/*===========================================================//
//    cross-platform breakpoint and static assertion macro   //
//===========================================================*/
#define MGF_STATIC_ASSERT(condition, message) static_assert(condition, message)

// 1. Visual Studio (MSVC)
#if defined(_MSC_VER)
#define MGF_PLATFORM_BREAK() __debugbreak()

// 2. x86/x64 아키텍처용 GCC 및 Clang (인라인 어셈블리)
#elif defined(__i386__) || defined(__x86_64__)
#define MGF_PLATFORM_BREAK() __asm__ volatile("int $3")

// 3. 현대적인 Clang/GCC (내장 함수 활용)
#elif defined(__has_builtin)
#if __has_builtin(__builtin_debugtrap)
#define MGF_PLATFORM_BREAK() __builtin_debugtrap()
#elif __has_builtin(__builtin_trap)
#define MGF_PLATFORM_BREAK() __builtin_trap()
#else
#include <cstdlib>
#define MGF_PLATFORM_BREAK() std::abort()
#endif

// 4. 모든 수단이 실패했을 때의 최종 방어선
#else
#include <cstdlib>
#define MGF_PLATFORM_BREAK() std::abort()
#endif
