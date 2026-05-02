#pragma once

namespace MGF3D
{
	struct alignas(16) SkinnedInstanceData
	{
		mat4 worldMatrix{ 1.0f };
		uint32 boneOffset{ 0 };  // 애니메이션 버퍼 시작 위치
		uint32 pad[3]{ 0 };      // 16바이트 정렬을 위한 패딩
	};
}