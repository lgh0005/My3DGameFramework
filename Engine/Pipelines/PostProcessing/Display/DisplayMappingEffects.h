#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(Texture)
#pragma endregion

enum class ToneMappingMode : uint8
{
	Exposure = 0, // SRP용 (기본, 선명함)
	ACES = 1, // URP용 (필름 느낌, 대비 강함)
	Reinhard = 2  // 범용 (차분함, 하이라이트 억제 좋음)
};

CLASS_PTR(DisplayMappingEffects)
class DisplayMappingEffects : public PostProcessEffect
{
	using Super = PostProcessEffect;

public:
	virtual ~DisplayMappingEffects() override;

	static DisplayMappingEffectsUPtr Create
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
	
	void SetToneMappingMode(ToneMappingMode mode) { m_toneMappingMode = mode; }
	void SetBloomStrength(float strength) { m_bloomStrength = strength; }
	void SetExposure(float exposure) { m_exposure = exposure; }

private:
	DisplayMappingEffects();
	bool Init(int32 priority, int32 width, int32 height);

	ProgramPtr m_compositeProgram;
	TexturePtr m_cameraDirtTexture;

	float m_bloomStrength{ 2.0f };

	ToneMappingMode m_toneMappingMode{ ToneMappingMode::Exposure };
	float m_gamma		{ 2.2f };
	float m_exposure	{ 1.0f };
	
	float m_cameraDirtAmbient{ 0.05f };
	float m_cameraDirtIntensity{ 2.0f };
};