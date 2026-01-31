#pragma once
#include "MeshRenderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(InstancedMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

// TODO : 이제 InstancedMeshRenderer는
// 마치 Terrain 위에 대량의 풀들을 렌더링 하는 특수 컴포넌트로 만들 필요가 있음
CLASS_PTR(InstancedMeshRenderer)
class InstancedMeshRenderer : public MeshRenderer
{
	using Super = MeshRenderer;
	DEFINE_COMPONENT_TYPE(ComponentType::InstancedMeshRenderer)

public:
	virtual ~InstancedMeshRenderer();
	static InstancedMeshRendererUPtr Create(InstancedMeshPtr mesh, MaterialPtr material);
	virtual RenderBounds GetWorldBounds() const override;
	virtual void Render(Program* program) const;
	virtual void UpdateInstanceProperty() override;

private:
	InstancedMeshRenderer();
	bool Init(InstancedMeshPtr mesh, MaterialPtr material);
	InstancedMesh* m_instancedMeshCache	{ nullptr };
};