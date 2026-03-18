#include "GraphicsPch.h"
#include "CameraUBO.h"

namespace MGF3D
{
	MGF_ASSERT_GPU_ALIGNMENT(CameraUBO);
	MGF_ASSERT_GPU_OFFSET(CameraUBO, viewPos, 16);
}