#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(ShadowMap)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(StandardShadowPass)
class StandardShadowPass : public RenderPass
{
public:
	static StandardShadowPassUPtr Create(int32 resolution = 1024);
	virtual void Render(Scene* scene, Camera* camera) override;

	glm::mat4 CalculateLightSpaceMatrix(Light* light);
	ShadowMap* GetShadowMap(int32 index) { return m_shadowMaps[index].get(); }
	void Resize(int32 widht, int32 height);

	// TEST : Context에 있는 내용물을 잘 렌더링 하는 지 테스트
	void TestRender(RenderContext* context);

private:
	StandardShadowPass() = default;
	bool Init(int32 resolution);

	int32	      m_resolution;
	std::vector<ShadowMapUPtr> m_shadowMaps;
	ProgramUPtr m_staticDepthProgram;
	ProgramUPtr m_skinnedDepthProgram;
};