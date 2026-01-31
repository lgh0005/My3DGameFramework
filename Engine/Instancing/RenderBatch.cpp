#include "EnginePch.h"
#include "RenderBatch.h"

void StaticRenderBatch::Clear() 
{ 
	instanceData.clear(); 
}

void StaticRenderBatch::Add(const StaticInstanceProperty& prop)
{
	instanceData.push_back(prop);
}

void SkinnedRenderBatch::Clear()
{
	instanceData.clear();
}

void SkinnedRenderBatch::Add(const SkinnedInstanceProperty& prop)
{
	instanceData.push_back(prop);
}
