#pragma once
#include "Instancing/RenderQueue.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
CLASS_PTR(Material)
CLASS_PTR(InstanceBuffer)
#pragma endregion

CLASS_PTR(StaticRenderQueue)
class StaticRenderQueue : public RenderQueue
{
	using MaterialBatch = std::unordered_map<Material*, std::vector<StaticInstanceProperty>>;
	using StaticMeshBatch = std::unordered_map<Mesh*, MaterialBatch>;

public:
	virtual ~StaticRenderQueue() override;
	static StaticRenderQueueUPtr Create(uint32 maxInstanceCount);

	virtual bool Init(uint32 maxInstanceCount) override;
	virtual void Clear() override;
	virtual void Execute(Program* prog) override;
	void Add(Mesh* mesh, Material* material, const StaticInstanceProperty& prop);

private:
	StaticRenderQueue();
	StaticMeshBatch    m_batches;
	InstanceBufferUPtr m_instanceBuffer;
};