#pragma once

namespace MGF3D
{
	class ProfileScope
	{
	public:
		ProfileScope(cstr* name);
		~ProfileScope();

	private:
		cstr* m_name;
		uint64 m_start;
	};
}

/*==========================//
//      Profiling macro     //
//==========================*/
#pragma region PROFILE_ELAPSED_TIME

#ifdef _DEBUG
// __LINE__을 사용하여 변수 이름이 겹치지 않게 자동으로 생성
#define MGF_PROFILE_SCOPE(name) MGF3D::ProfileScope profile_##__LINE__(name)
#else
#define MGF_PROFILE_SCOPE(name) ((void)0)
#endif

#pragma endregion