#pragma once
#include "PostProcessing/PostProcessEffect.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)

	enum class EToneMappingMode : uint8
	{
		Exposure = 0,
		ACES,
		Reihard,
		None
	};

	MGF_CLASS_PTR(LDREffects)
	class LDREffects : public PostProcessEffect
	{
		using Super = PostProcessEffect;

	public:
		virtual ~LDREffects();
		static LDREffectsUPtr Create();

	public:
		virtual bool Render(RenderContext* context) override;
		virtual void Resize(int32 width, int32 height) override;

	public:
		LDREffects();
		bool Init();

		// LDR-effect program
		GraphicsProgramPtr m_lowDynmaicRangeProgram{ nullptr };

		// Tone-mapping effect params
		EToneMappingMode m_toneMappingMode{ EToneMappingMode::None };
		float m_exposure{ 1.0f };
		float m_gamma{ 0.0f };

		// Vignette effect params
		float m_vignetteIntensity{ 0.05f };

		// Sharpen effect params
		float m_sharpenAmount{ 0.2f };

		// Film Grain noise effect params
		float m_grainAmount{ 0.01f };

		// CRT Screen effect params
		vec2  m_distortionK{ 0.08f, 0.04f };
		float m_scanlineIntensity{ 0.45f };
		float m_phosphorIntensity{ 0.0f };
		float m_flickerIntensity{ 0.0005f };
	};
}

