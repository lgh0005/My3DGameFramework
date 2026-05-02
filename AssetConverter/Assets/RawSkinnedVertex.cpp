#include "CorePch.h"
#include "RawSkinnedVertex.h"

namespace MGF3D
{
	RawSkinnedVertex::RawSkinnedVertex()
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			boneIDs[i] = -1;
			weights[i] = 0.0f;
		}
	}
}