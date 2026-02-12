#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Meshes/Mesh.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(ScreenMesh)
class ScreenMesh : public Mesh
{
	DEFINE_RESOURCE_TYPE(ResourceType::ScreenMesh, ScreenMeshDesc)

public:
	virtual ~ScreenMesh() override;
	static ScreenMeshUPtr Create();
	virtual void Draw() const override;

private:
	ScreenMesh();
	void Init();
};