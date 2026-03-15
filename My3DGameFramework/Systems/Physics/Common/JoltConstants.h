#pragma once

namespace MGF3D
{
	/*=======================//
	//   Jolt 초기화 설정값   //
	//=======================*/
	namespace JoltConfig
	{
		static constexpr JPH::uint cMaxBodies			  = 65536;
		static constexpr JPH::uint cNumBodyMutexes		  = 0;
		static constexpr JPH::uint cMaxBodyPairs		  = 65536;
		static constexpr JPH::uint cMaxContactConstraints = 10240;
		static constexpr JPH::uint cMaxPhysicsJobs		  = 2048;				// 최대 물리 잡(Job) 수
		static constexpr JPH::uint cMaxPhysicsBarriers	  = 8;					// 최대 배리어 수 (보통 8이면 충분)
		static constexpr usize     cTempAllocatorSize     = 10 * 1024 * 1024;
	}
}