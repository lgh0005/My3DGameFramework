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
		// Tone Mapping
		EToneMappingMode GetToneMappingMode() const noexcept { return m_toneMappingMode; }
		void SetToneMappingMode(EToneMappingMode mode) noexcept { m_toneMappingMode = mode; }

		float GetExposure() const noexcept { return m_exposure; }
		void SetExposure(float exposure) noexcept
		{
			m_exposure = std::max(0.0f, exposure);
		}

		float GetGamma() const noexcept { return m_gamma; }
		void SetGamma(float gamma) noexcept
		{
			m_gamma = std::max(0.0f, gamma);
		}

		// Vignette
		float GetVignetteIntensity() const noexcept { return m_vignetteIntensity; }
		void SetVignetteIntensity(float intensity) noexcept
		{
			m_vignetteIntensity = std::clamp(intensity, 0.0f, 1.0f);
		}

		// Sharpen
		float GetSharpenAmount() const noexcept { return m_sharpenAmount; }
		void SetSharpenAmount(float amount) noexcept
		{
			m_sharpenAmount = std::max(0.0f, amount);
		}

		// Film Grain
		float GetGrainAmount() const noexcept { return m_grainAmount; }
		void SetGrainAmount(float amount) noexcept
		{
			m_grainAmount = std::clamp(amount, 0.0f, 1.0f);
		}

		// CRT Screen
		vec2 GetDistortionK() const noexcept { return m_distortionK; }
		void SetDistortionK(const vec2& distortionK) noexcept
		{
			m_distortionK = distortionK;
		}

		float GetScanlineIntensity() const noexcept { return m_scanlineIntensity; }
		void SetScanlineIntensity(float intensity) noexcept
		{
			m_scanlineIntensity = std::clamp(intensity, 0.0f, 1.0f);
		}

		float GetPhosphorIntensity() const noexcept { return m_phosphorIntensity; }
		void SetPhosphorIntensity(float intensity) noexcept
		{
			m_phosphorIntensity = std::clamp(intensity, 0.0f, 1.0f);
		}

		float GetFlickerIntensity() const noexcept { return m_flickerIntensity; }
		void SetFlickerIntensity(float intensity) noexcept
		{
			m_flickerIntensity = std::clamp(intensity, 0.0f, 1.0f);
		}


	public:
		LDREffects();
		bool Init();

		// LDR-effect program
		GraphicsProgramPtr m_lowDynmaicRangeProgram{ nullptr };

		// Tone-mapping effect params
		EToneMappingMode m_toneMappingMode{ EToneMappingMode::None };
		float m_exposure{ 1.0f };
		float m_gamma{ 2.2f };

		// Vignette effect params
		float m_vignetteIntensity{ 0.05f };

		// Sharpen effect params
		float m_sharpenAmount{ 0.0f };

		// Film Grain noise effect params
		float m_grainAmount{ 0.01f };

		// CRT Screen effect params
		vec2  m_distortionK{ 0.08f, 0.04f };
		float m_scanlineIntensity{ 0.45f };
		float m_phosphorIntensity{ 0.0f };
		float m_flickerIntensity{ 0.0005f };
	};
}

