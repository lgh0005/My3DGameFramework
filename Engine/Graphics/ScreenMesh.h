#pragma once
#include "Graphics/Mesh.h"

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
	virtual MeshType GetMeshType() const override { return MeshType::Screen; }

	virtual void Draw(const Program* program = nullptr) const override;

private:
	ScreenMesh() = default;
	void Init();
};