#include "GraphicsPch.h"
#include "GraphicsUtils.h"

namespace MGF3D
{
	const mat4& GraphicsUtils::GetIdentityMatrix()
	{
		static const mat4 identity(1.0f);
		return identity;
	}
}