#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(EffectFramebuffer)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(KawaseBloomEffect)
class KawaseBloomEffect : public PostProcessEffect
{
	using Super = PostProcessEffect;
	struct KawaseBloomMips
	{
		TexturePtr texture;
		int32 width; int32 height;
	};

public:
	virtual ~KawaseBloomEffect() override;
	static KawaseBloomEffectUPtr Create
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
	KawaseBloomEffect();
	bool Init(int32 priority, int32 width, int32 height);
	void CreateBloomMips(int32 width, int32 height);

	ProgramPtr				     m_bloomProgram;
	EffectFramebufferUPtr		 m_bloomFBO;
	std::vector<KawaseBloomMips> m_bloomMips;
	float m_threshold		{ 1.0f };
	int32 m_iteration		{ 5 };
};