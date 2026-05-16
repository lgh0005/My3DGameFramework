#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(EffectFramebuffer)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(DOFEffect)
class DOFEffect : public PostProcessEffect
{
	using Super = PostProcessEffect;

public:
	virtual ~DOFEffect();
	static DOFEffectUPtr Create
	(
		int32 priority,
		int32 width = WINDOW_WIDTH,
		int32 height = WINDOW_HEIGHT
	);

	virtual bool Render
	(
		RenderContext* context,
		Framebuffer* src,
		Framebuffer* dst,
		ScreenMesh* scrn
	) override;

	virtual void OnResize(int32 width, int32 height) override;

private:
	DOFEffect();
	bool Init(int32 priority, int32 width, int32 height);

	ProgramPtr m_DepthOfFieldProgram;

	// DOF 조절값
	float m_focusDistance{ 3.75f };    // 초점이 맞는 거리 (World Unit)
	float m_focusRange{ 7.5f };       // 초점이 선명한 구간의 너비
	float m_bokehRadius{ 3.0f };      // 흐림 강도 (Pixel 단위 반경)
};