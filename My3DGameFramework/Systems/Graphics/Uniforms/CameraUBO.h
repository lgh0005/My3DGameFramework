#pragma once

namespace MGF3D
{
	struct alignas(OPENGL_MEMORY_ALIGNMENT) CameraUBO
	{
		mat4  view;
		mat4  projection;
		vec3  viewPos;			float pad0;
	};
}