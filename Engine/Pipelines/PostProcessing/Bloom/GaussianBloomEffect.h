#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(GaussianBloomEffect)
class GaussianBloomEffect : public PostProcessEffect
{
	using Super = PostProcessEffect;

public:
	virtual ~GaussianBloomEffect() override;
	static GaussianBloomEffectUPtr Create
	(
		int32 priority,
		int32 width = WINDOW_WIDTH,
		int32 height = WINDOW_HEIGHT
	);
	virtual bool Render
	(
		RenderContext* context,
		Framebuffer* srcFBO,
		Framebuffer* dstFBO,
		ScreenMesh* screenMesh
	) override;

	virtual void OnResize(int32 width, int32 height) override;

private:
	GaussianBloomEffect();
	bool Init(int32 priority, int32 width, int32 height);
	void ExtractBrightAreas(Texture* src, ScreenMesh* mesh);
	Texture* ComputeGaussianBlur(ScreenMesh* mesh);

	std::array<PostProcessFramebufferUPtr, 2> m_pingPongFBOs; // 블러용 핑퐁 버퍼
	ProgramPtr m_thresholdProgram; // 밝은 영역 추출 셰이더
	ProgramPtr m_blurProgram;      // 가우시안 블러 셰이더
};