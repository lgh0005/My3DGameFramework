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

	// 블룸 세기
	float m_bloomStrength{ 2.0f };

	// 톤매핑 및 감마 보정
	ToneMappingMode m_toneMappingMode{ ToneMappingMode::Exposure };
	float m_gamma		{ 2.2f };
	float m_exposure	{ 1.0f };
	
	// Camera dirt
	TexturePtr m_cameraDirtTexture;
	float m_cameraDirtAmbient{ 0.05f };
	float m_cameraDirtIntensity{ 2.0f };

	// CRT screen effect
	glm::vec2 m_distortionK{ 0.0f, 0.0f }; // CRT 곡률
	float m_scanlineIntensity{ 0.0f };		 // 가로줄
	float m_phosphorIntensity{ 0.0f };		 // 세로 격자
	float m_flickerIntensity{ 0.0f };	     // 미세 떨림(약간의 깜빡임)

	// 색수차
	float m_chromaticAmount{ 0.0f };       // 색수차 강도

	// vignette
	float m_vignetteIntensity{ 0.0f };     // 비네팅 강도

	// 필름 그레인
	float m_grainAmount{ 0.0f };           // 필름 그레인 강도

	// 선명도 강도
	float m_sharpenAmount{ 0.0f };          // 선명도 강도
};