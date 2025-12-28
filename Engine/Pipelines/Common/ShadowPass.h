#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Program)
CLASS_PTR(ShadowMap)
CLASS_PTR(StaticMeshRenderer)
CLASS_PTR(SkinnedMeshRenderer)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(ShadowPass)
class ShadowPass : public ContextRenderPass
{
public:
	static ShadowPassUPtr Create(int32 resolution = 1024);
	virtual void Render(RenderContext* context) override;
	ShadowMap* GetShadowMap(int32 index) { return m_shadowMaps[index].get(); }
	void Resize(int32 widht, int32 height);

/*==================================//
//   shadow casting helper methods  //
//==================================*/
private:
	ShadowPass() = default;
	bool Init(int32 resolution);

	glm::mat4 CalculateLightSpaceMatrix(Light* light);
	void RenderStaticMeshes
	(
		const std::vector<StaticMeshRenderer*>& meshes,
		const glm::mat4& lightSpaceMatrix
	);

	void RenderSkinnedMeshes
	(
		const std::vector<SkinnedMeshRenderer*>& meshes,
		const glm::mat4& lightSpaceMatrix
	);

	void RegisterShadowMapsToContext(RenderContext* context);

	int32	    m_resolution;
	std::vector<ShadowMapUPtr> m_shadowMaps;
	ProgramUPtr m_staticDepthProgram;
	ProgramUPtr m_skinnedDepthProgram;
};