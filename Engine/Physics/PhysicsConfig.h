#pragma once
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

// 1. Jolt 초기화 설정값
namespace JoltConfig
{
	constexpr JPH::uint cMaxBodies = 65536;
	constexpr JPH::uint cNumBodyMutexes = 0;
	constexpr JPH::uint cMaxBodyPairs = 65536;
	constexpr JPH::uint cMaxContactConstraints = 10240;
	constexpr JPH::uint cMaxPhysicsJobs = 2048;		// 최대 물리 잡(Job) 수
	constexpr JPH::uint cMaxPhysicsBarriers = 8;	// 최대 배리어 수 (보통 8이면 충분)
}

// 2. 오브젝트 레이어 (게임 로직용)
namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0; // 벽, 바닥
	static constexpr JPH::ObjectLayer MOVING	 = 1; // 플레이어, 적
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

// 3. 브로드페이즈 레이어 (물리 엔진 내부 최적화용)
namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::uint			  NUM_LAYERS(2);
};