#pragma once
#include "Instancing/InstanceProperty.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Material)
#pragma endregion

struct RenderBatch
{
	Mesh* mesh;
	Material* material;
};

struct StaticRenderBatch : RenderBatch
{
	void Clear();
	void Add(const StaticInstanceProperty& prop);
	std::vector<StaticInstanceProperty> instanceData;
};

struct SkinnedRenderBatch : RenderBatch
{
	void Clear();
	void Add(const SkinnedInstanceProperty& prop);
	std::vector<SkinnedInstanceProperty> instanceData;
	std::vector<glm::mat4> boneData;
};
