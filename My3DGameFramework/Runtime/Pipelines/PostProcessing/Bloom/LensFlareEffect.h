#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(EffectFramebuffer)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(LensFlareEffect)
class LensFlareEffect : public PostProcessEffect
{
public:
	virtual ~LensFlareEffect();
	static LensFlareEffectUPtr Create
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

private:
	LensFlareEffect();
	bool Init(int32 priority, int32 width, int32 height);

	ProgramPtr m_lensFlareProgram;

	// Lens Flare 조절값
	int32 m_ghostCount{ 4 };         // 반사광(Ghost)의 개수
	float m_ghostSpacing{ 0.35f };   // 고스트 간의 거리
	float m_haloRadius{ 0.15f };      // 화면 중앙 무지개 링의 반지름
	float m_flareIntensity{ 0.75f };  // 전체 플레어 강도
};