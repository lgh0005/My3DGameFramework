#pragma once

/*======================================//
//    cross-platform breakpoint macro   //
//======================================*/
#pragma region BREAK_POINT

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
#include <cstdlib>
#define MGF_PLATFORM_BREAK() std::abort()
#endif

// 모든 수단이 실패했을 때의 표준 방어선
#else
#include <cstdlib>
#define MGF_PLATFORM_BREAK() std::abort()
#endif

#pragma endregion

namespace MGF3D
{
	class Asserter
	{
		DECLARE_UTILITIES(Asserter)

	public:
		static void ReportFailure
		(
			const char* condition, 
			const char* message, 
			const char* file, int32 line
		);
	};
}

/*======================//
//    Assertion macro   //
//======================*/
#pragma region ASSERTION

#define MGF_STATIC_ASSERT(condition, message) static_assert(condition, message)

#ifdef _DEBUG
#define MGF_ASSERT(condition, message)												 \
do {																				 \
        if (!(!!(condition)))														 \
		{																			 \
            MGF3D::Asserter::ReportFailure(#condition, message, __FILE__, __LINE__); \
            MGF_PLATFORM_BREAK();													 \
        }																			 \
    } while (0)
#define MGF_ASSERT_BASE(condition) MGF_ASSERT(condition, "No Message")
#else
#define MGF_ASSERT(condition, message) ((void)0)
#define MGF_ASSERT_BASE(condition)     ((void)0)
#endif

#pragma endregion