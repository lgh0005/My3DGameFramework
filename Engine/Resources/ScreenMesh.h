#pragma once
#include "Resources/Mesh.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(ScreenMesh)
class ScreenMesh : public Mesh
{
public:
	virtual ~ScreenMesh() override;
	static ScreenMeshUPtr Create();
	static const ResourceType s_ResourceType = ResourceType::ScreenMesh;
	virtual ResourceType GetResourceType() const override { return ResourceType::ScreenMesh; }
	virtual void Draw() const override;

private:
	ScreenMesh();
	void Init();
};