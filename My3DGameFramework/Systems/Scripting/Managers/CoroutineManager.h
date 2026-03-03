#pragma once

namespace MGF3D
{
	class CoroutineManager
	{
		DECLARE_SINGLE(CoroutineManager)

	public:
		CoroutineManager();
		~CoroutineManager();

	public:

		// TODO : LuaVM을 바탕으로 같이 연동해야 함.
		// 추가로, 이와 더불어서 Lua에서 선언한 코루틴을 이 단에서 
		// 통신 또는 대응되는 클래스를 만들 필요가 있음.
		// 1. Coroutine 클래스
		// 2. YieldInstructor 
		// ..etc

	private:


	};
}