#pragma once
#include "Instancing/InstanceProperty.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Material)
#pragma endregion

struct RenderBatch
{
	void Clear();
	void Add
	(
		const InstanceProperty& prop,
		const std::vector<glm::mat4>& bones = {}
	);

	Mesh* mesh;
	Material* material;
	std::vector<InstanceProperty> instanceData;
	std::vector<glm::mat4> boneData;
};

