#pragma once
#include "Spatial/Position.h"
#include "Spatial/Rotation.h"
#include "Spatial/Scale.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GameObject)
	MGF_CLASS_PTR(Transform)

	struct TransformDescriptor
	{
		TransformDescriptor(Ptr<GameObject> _owner);
		~TransformDescriptor();

		// 1. 소유자 정보 (공간의 주인)
		Ptr<GameObject> owner{ nullptr };

		// 2. 초기 공간 데이터 (Position, Rotation, Scale)
		Position position	{};
		Rotation rotation	{};
		Scale	 scale		{};

		// 3. 계층 구조 초기 설정
		Ptr<Transform> parent{ nullptr };
	};
}