#include "EnginePch.h"
#include "RenderBatch.h"

void RenderBatch::Clear()
{
    instanceData.clear();
    boneData.clear();
}

void RenderBatch::Add
(
	const InstanceProperty& prop,
	const std::vector<glm::mat4>& bones
)
{
	InstanceProperty finalProp = prop;

    // 1. Skinned Mesh 로직
    if (!bones.empty())
    {
        finalProp.typeFlag = 1;
        finalProp.boneOffset = static_cast<uint32>(boneData.size());
        boneData.insert(boneData.end(), bones.begin(), bones.end());
    }

    // 2. Static Mesh 로직
    else
    {
        finalProp.typeFlag = 0;
        finalProp.boneOffset = 0;
    }

    instanceData.push_back(finalProp);
}
