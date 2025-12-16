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
	static ScreenMeshUPtr Create();
	virtual ~ScreenMesh() override;
	static const ResourceType s_ResourceType = ResourceType::ScreenMesh;
	virtual ResourceType GetResourceType() const override { return ResourceType::ScreenMesh; }
	virtual void Draw(const Program* program = nullptr) const override;

private:
	ScreenMesh() = default;
	void Init();
};