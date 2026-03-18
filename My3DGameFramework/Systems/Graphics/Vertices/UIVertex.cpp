#include "GraphicsPch.h"
#include "UIVertex.h"

namespace MGF3D
{
    MGF_ASSERT_GPU_ALIGNMENT(UIVertex);
    MGF_ASSERT_GPU_OFFSET(UIVertex, color, 16);
}