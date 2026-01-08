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
// INFO : 유니티의 충돌 레이어와 비슷함. 
// 예를 들어, Wall, Player, Enemy, Bullet, Item 등과 같이 게임 기획상 충돌 여부를 판단하기 
// 위한 기준이다. 여기에서는 브로드페이즈 레이어와 똑같이 둬서 "정적인 대상"과 "움직이는 대상"
// 이 두 레이어로 구분한다.
namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING	 = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

// 3. 브로드페이즈 레이어 (물리 엔진 내부 최적화용)
// INFO : 충돌 검사를 할 때 크게 "움직이는 대상"과 "정적인 대상" 이 두 기준으로 나눠서
// i) 정적인 대상 vs. 정적인 대상 : 이들은 충돌 검사를 할 이유가 없으니 패스
// ii) 동적인 대상 : 이들은 정적인 대상과 동적인 대상 모두와 충돌 검사를 수행해야 한다.
// 따라서, 크게 PhaseLayer는 NON_MOVIGN, MOVING으로 구분한다.
namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::uint			  NUM_LAYERS(2);
}