#pragma once
#include "LightUBO.h"

namespace MGF3D
{
	struct alignas(OPENGL_MEMORY_ALIGNMENT) LightArrayUBO
	{
        LightUBO  lights[MAX_LIGHTS];
        vec3      viewPos;
        int32     lightCount;
	};
}