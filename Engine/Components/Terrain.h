#pragma once
#include "Components/Component.h"

CLASS_PTR(Terrain)
class Terrain : public Component
{
public:
	virtual ~Terrain();
	static TerrainUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Terrain;
	virtual ComponentType GetComponentType() const override { return ComponentType::Terrain; }

	// TODO 
	// 1. Height Map을 받아서 지형 생성
	// 2. Jolt의 JPH::HeightFieldShape을 이용한 지형 기반 충돌
	// 모양 구현 필요

private:
	Terrain();

};