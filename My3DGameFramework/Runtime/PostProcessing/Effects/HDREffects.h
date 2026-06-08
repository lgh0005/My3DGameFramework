#pragma once
#include "PostProcessing/PostProcessEffect.h"
#include "Textures/GLTexture2D.h"
#include "Framebuffers/GLFramebuffer2D.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)

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
		virtual bool Render(RenderContext* context) override;
		virtual void Resize(int32 width, int32 height) override;

	private:
		HDREffects();
		bool Init();

		// HDR-effect program
		GraphicsProgramPtr m_highDynamicRangeProgram{ nullptr };

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
		float m_threshold { 1.0f };
		int32 m_iteration { 5 };

		// Gaussian-bloom effect params
		Array<GLFramebuffer2DUPtr, 2> m_gaussianBloomFBOs;
		GraphicsProgramPtr m_gaussainThresholdProgram;
		GraphicsProgramPtr m_gaussianBlurProgram;

		// Kawase-bloom effect params
		struct KawaseBloomMips
		{
			GLTexture2D texture;
			int32 width; 
			int32 height;
		};
		Vector<KawaseBloomMips> m_kawaseBloomMips;
		GLFramebuffer2DUPtr m_kawaseBloomFBO;
		GraphicsProgramPtr m_kawaseBloomProgram;
	};
}