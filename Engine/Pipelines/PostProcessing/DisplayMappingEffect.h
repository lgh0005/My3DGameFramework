#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(DisplayMappingEffect)
class DisplayMappingEffect : public PostProcessEffect
{
	using Super = PostProcessEffect;

public:
	virtual ~DisplayMappingEffect() override;

	static DisplayMappingEffectUPtr Create
	(
		int32 priority,
		int32 width = WINDOW_WIDTH,
		int32 height = WINDOW_HEIGHT
	);

	virtual bool Render
	(
		RenderContext* context,
		Framebuffer* mainFBO,
		ScreenMesh* screenMesh
	) override;

	virtual void OnResize(int32 width, int32 height) override;

private:
	DisplayMappingEffect();
	bool Init(int32 priority, int32 width, int32 height);

	PostProcessFramebufferUPtr m_resultFBO;
	ProgramPtr m_compositeProgram;
	TexturePtr m_cameraDirtTexture;

	float m_gamma = 2.2f;
	float m_exposure = 1.0f;
};