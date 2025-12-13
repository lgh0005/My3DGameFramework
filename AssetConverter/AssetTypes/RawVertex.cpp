#include "pch.h"
#include "RawVertex.h"

RawSkinnedVertex::RawSkinnedVertex()
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) 
    {
        boneIDs[i] = -1;
        weights[i] = 0.0f;
    }
}
