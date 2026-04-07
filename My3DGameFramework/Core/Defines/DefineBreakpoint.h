#pragma once
#include <cstdlib>

/*======================================//
//    cross-platform breakpoint macro   //
//======================================*/
// 디버그 빌드일 때만 중단점 활성화
#if defined(_DEBUG)

    // Visual Studio용 최적화된 중단점
    #if defined(_MSC_VER)
        #define MGF_PLATFORM_BREAK() __debugbreak()

    // GCC/Clang x86 아키텍처용 소프트웨어 인터럽트
    #elif defined(__i386__) || defined(__x86_64__)
        #define MGF_PLATFORM_BREAK() __asm__ volatile("int $3")

    // Clang/최신 GCC: 디버그 트랩이 있으면 우선 사용하고, 없으면 일반 트랩 사용
    #elif defined(__has_builtin)
        #if __has_builtin(__builtin_debugtrap)
            #define MGF_PLATFORM_BREAK() __builtin_debugtrap()
        #elif __has_builtin(__builtin_trap)
            #define MGF_PLATFORM_BREAK() __builtin_trap()
        #else
            #define MGF_PLATFORM_BREAK() std::abort()
        #endif

    // 모든 수단이 실패했을 때의 표준 방어선
    #else
    #define MGF_PLATFORM_BREAK() std::abort()
    #endif

// 릴리즈(배포) 빌드에서는 무시되도록 처리
#else
#define MGF_PLATFORM_BREAK() ((void)0)
#endif