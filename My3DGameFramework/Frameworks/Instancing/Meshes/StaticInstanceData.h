#pragma once

namespace MGF3D
{
	struct alignas(16) StaticInstanceData
	{
		mat4 worldMatrix{ 1.0f };
	};
}