#pragma once

namespace MGF3D
{
	struct alignas(16) InstanceProperty
	{
		mat4 worldMatrix { 1.0f };
	};
}