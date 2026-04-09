#pragma once
#include "Instancing/RenderBatch.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(InstanceBuffer)
CLASS_PTR(Mesh)
CLASS_PTR(Material)
#pragma endregion

CLASS_PTR(RenderQueue)
class RenderQueue
{
	using MaterialMap = std::unordered_map<Material*, RenderBatch>;
	using MeshMap = std::unordered_map<Mesh*, MaterialMap>;

public:
	virtual ~RenderQueue();
	static RenderQueueUPtr Create(uint32 maxInstanceCount, uint32 maxBoneCount);

	bool Init(uint32 maxInstanceCount, uint32 maxBoneCount);
	void Clear();
	void Execute(Program* prog);

	void Add
	(
		Mesh* mesh,
		Material* material,
		const InstanceProperty& prop,
		const std::vector<glm::mat4>& bones = {}
	);

protected:
	RenderQueue();

	MeshMap            m_batches;
	InstanceBufferUPtr m_instanceBuffer; // Slot 0: 통합 속성
	InstanceBufferUPtr m_boneBuffer;     // Slot 1: 뼈 행렬
};