#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Meshes/Mesh.h"
#include "Graphics/Layouts/Vertex.h"

CLASS_PTR(UIMesh)
class UIMesh : public Mesh
{
	DEFINE_RESOURCE_TYPE(ResourceType::UIMesh, UIMeshDesc)

public:
	virtual ~UIMesh();
	static UIMeshUPtr Create();
	virtual void Draw() const override;

	void SetVertices(const std::vector<UIVertex>& vertices);
	void SetIndices(const std::vector<uint32>& indices);

private:
	UIMesh();
	void Init();
};