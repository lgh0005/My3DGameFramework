#pragma once
#include "Debug/RealTimer.h"

namespace MGF3D
{
	class ProfileScope
	{
	public:
		ProfileScope(cstr name);
		~ProfileScope();

	private:
		cstr m_name;
		uint64 m_start;
	};
}

/*==============================//
//      Profiling Macros        //
//==============================*/
#pragma region PROFILE_ELAPSED_TIME

#ifdef _DEBUG
// 1. 스코프 방식: 중괄호 블록의 시작과 끝을 자동으로 측정
#define MGF_PROFILE_SCOPE(name) MGF3D::ProfileScope profile_##__LINE__(name)

// 2. 수동 지점 방식: 시작과 끝을 직접 지정 (태그 변수 생성)
#define MGF_PROFILE_START(tag) uint64 __profile_start_##tag = MGF3D::RealTimer::_Internal_GetTimestamp()
#define MGF_PROFILE_END(tag) \
do { \
       uint64 __profile_end_##tag = MGF3D::RealTimer::_Internal_GetTimestamp(); \
       double __elapsed_##tag = MGF3D::RealTimer::_Internal_GetElapsedSeconds(__profile_start_##tag, __profile_end_##tag); \
       MGF_LOG_INFO("[Profile] {0} took {1} seconds", #tag, __elapsed_##tag); \
   } while (0)
#else
// 릴리즈 빌드에서는 모든 프로파일링 코드가 증발합니다.
#define MGF_PROFILE_SCOPE(name)    ((void)0)
#define MGF_PROFILE_START(tag)     ((void)0)
#define MGF_PROFILE_END(tag)       ((void)0)
#endif

#pragma endregion