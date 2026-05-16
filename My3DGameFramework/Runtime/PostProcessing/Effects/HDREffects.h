#pragma once
#include "PostProcessing/PostProcessEffect.h"

namespace MGF3D
{
	enum class EBloomMode : uint8
	{
		None = 0,
		Gaussian,
		Kawase
	};

	MGF_CLASS_PTR(HDREffects)
	class HDREffects : public PostProcessEffect
	{
		using Super = PostProcessEffect;

	public:
		virtual ~HDREffects();
		static HDREffectsUPtr Create();

	public:
		virtual bool Render
		(
			RenderContext* context,
			GLFramebuffer2D* srcFBO,
			GLFramebuffer2D* dstFBO
		) override;
		virtual void Resize(int32 width, int32 height) override;

	public:
		HDREffects();
		bool Init();

		// Depth of Field effect params
		float m_focusDistance{ 10.0f };
		float m_focusRange{ 5.0f };
		float m_dofBlurRadius{ 4.0f };

		// Lens flare effect params
		float m_flareThreshold{ 1.5f };
		float m_flareIntensity{ 1.0f };
		int32 m_ghostCount{ 3 };
		float m_ghostDispersal{ 0.3f };
		float m_haloWidth{ 0.4f };

		// Bloom params
		EBloomMode m_bloomMode{ EBloomMode::None };

		// Gaussian-bloom effect params


		// Kawase-bloom effect params

	};
}