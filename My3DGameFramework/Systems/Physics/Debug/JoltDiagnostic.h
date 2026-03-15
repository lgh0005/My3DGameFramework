#pragma once

namespace MGF3D
{
	class JoltDiagnostic
	{
		MGF_DECLARE_UTILITIES(JoltDiagnostic)

	public:
		static void Bind();

	private:
		static void OnJoltTrace(cstr inFMT, ...);
		static bool OnJoltAssertFailed
		(
			cstr inExpression,
			cstr inMessage,
			cstr inFile,
			uint32 inLine
		);
	};
}

/*==============================================//
//    Jolt Debug/Assert Binding Macros          //
//==============================================*/
#ifdef JPH_ENABLE_ASSERTS
	#define MGF_JPH_ASSERT_BIND(func) JPH::AssertFailed = func
#else
	#define MGF_JPH_ASSERT_BIND(func) ((void)0)
#endif

// 트레이스(로그)도 동일하게 처리 가능
#define MGF_JPH_TRACE_BIND(func)  JPH::Trace = func