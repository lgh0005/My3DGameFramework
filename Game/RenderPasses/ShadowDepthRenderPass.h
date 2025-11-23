#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Framebuffer)
CLASS_PTR(ShadowMap)
#pragma endregion

CLASS_PTR(ShadowDepthRenderPass)
class ShadowDepthRenderPass : public ShadowPass
{
public:
	static ShadowDepthRenderPassUPtr Create
	(
		ProgramUPtr staticDepthProgram,   // 정적 메쉬용 뎁스 셰이더
		ProgramUPtr skinnedDepthProgram,  // 스키닝 메쉬용 뎁스 셰이더
		int32 resolution                  // 그림자 맵 해상도 (예: 1024)
	);
	virtual void Render(Scene* scene, Camera* camera) override;
	virtual void CalculateLightSpaceMatrix(Scene* scene) override;

private:
	ShadowDepthRenderPass() = default;
	bool Init
	(
		ProgramUPtr staticDepthProgram,
		ProgramUPtr skinnedDepthProgram,
		int32 resolution
	);

	FramebufferUPtr m_gBuffer;
	ProgramUPtr m_staticDepthProgram;
	ProgramUPtr m_skinnedDepthProgram;
};
