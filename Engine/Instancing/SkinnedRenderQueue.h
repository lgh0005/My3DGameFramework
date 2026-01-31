#pragma once
#include "Instancing/RenderQueue.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(SkinnedMesh)
CLASS_PTR(Material)
CLASS_PTR(InstanceBuffer)
#pragma endregion

CLASS_PTR(SkinnedRenderQueue)
class SkinnedRenderQueue : public RenderQueue
{
public:
	using MaterialMap = std::unordered_map<Material*, SkinnedRenderBatch>;
	using SkinnedMeshMap = std::unordered_map<SkinnedMesh*, MaterialMap>;

public:
	virtual ~SkinnedRenderQueue() override;
	static SkinnedRenderQueueUPtr Create(uint32 maxInstanceCount, uint32 maxBoneCount);
	
	virtual bool Init(uint32 maxInstanceCount) override;
	bool Init(uint32 maxInstanceCount, uint32 maxBoneCount);
	virtual void Clear() override;
	virtual void Execute(GraphicsProgram* prog) override;

	void Add
	(
		SkinnedMesh* mesh, Material* material, 
		SkinnedInstanceProperty prop, 
		const std::vector<glm::mat4>& boneMatrices
	);

private:
	SkinnedRenderQueue();

	SkinnedMeshMap     m_batches;
	InstanceBufferUPtr m_instanceBuffer; // Slot 0: WorldMatrix, boneOffset 등
	InstanceBufferUPtr m_boneBuffer;     // Slot 1: 전역 Bone Matrices
};
