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
	static ScreenMeshPtr Create();
	static ScreenMeshPtr Load(const ScreenMeshDesc& desc);
	virtual ScreenMeshDesc& GetDesc() override { return m_desc; }
	virtual const ResourceDesc& GetDesc() const override { return m_desc; }
	virtual void Draw() const override;

private:
	ScreenMesh();
	void Init();
	ScreenMeshDesc m_desc;
};