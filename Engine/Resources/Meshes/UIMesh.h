#pragma once
#include "Resources/Meshes/Mesh.h"
#include "Graphics/Vertex.h"

CLASS_PTR(UIMesh)
class UIMesh : public Mesh
{
public:
	virtual ~UIMesh();
	static UIMeshUPtr Create();
	static const ResourceType s_ResourceType = ResourceType::UIMesh;
	virtual ResourceType GetResourceType() const override { return ResourceType::UIMesh; }
	virtual void Draw() const override;

	void SetVertices(const std::vector<UIVertex>& vertices);
	void SetIndices(const std::vector<uint32>& indices);

private:
	UIMesh();
	void Init();
};