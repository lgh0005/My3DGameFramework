#pragma once
//#include "Object/Component.h"

//CLASS_PTR(Terrain)
//class Terrain : public Component
//{
//	DEFINE_COMPONENT_TYPE(ComponentType::Terrain)
//
//public:
//	virtual ~Terrain();
//	static TerrainUPtr Create();
//
//	// TODO 
//	// 1. Height Map을 받아서 지형 생성
//	// 2. Jolt의 JPH::HeightFieldShape을 이용한 지형 기반 충돌
//	// 모양 구현 필요
//
//private:
//	Terrain();
//
//};

// TODO : 
// Terrain은 더 이상 컴포넌트가 아니라 하나의 리소스 데이터로 존재해야 함
// 1. Renderer -> GraphicsLib.lib에서 TerrainRenderer 컴포넌트를 구현
// 2. Physics -> PhysicsLib.lib에서 TerrainCollier 컴포넌트를 구현
// 3. AI -> AILIb.lib에서 TerrainNavMesh 컴포넌트를 구현
// 즉, Terrain은 이 컴포넌트 구현을 위한 데이터가 되어줘야 함.

