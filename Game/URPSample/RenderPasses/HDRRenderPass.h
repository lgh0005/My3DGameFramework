#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(StaticMeshRenderer)
#pragma endregion

// TODO : 이후에 URP의 DeferredLightingPass로 승격되어야 할 렌더 패스
CLASS_PTR(HDRRenderPass)
class HDRRenderPass : public GeneralRenderPass
{
public:
	virtual ~HDRRenderPass();
	static HDRRenderPassUPtr Create(ProgramUPtr program);
	virtual void Render(Scene* scene, Camera* camera) override;

	const std::vector<StaticMeshRenderer*>& GetRenderers() const;
	void AddRenderer(StaticMeshRenderer* meshRenderer);

private:
	HDRRenderPass();
	bool Init(ProgramUPtr program);

	ProgramUPtr m_simpleProgram;
	std::vector<StaticMeshRenderer*> m_renderers;
};