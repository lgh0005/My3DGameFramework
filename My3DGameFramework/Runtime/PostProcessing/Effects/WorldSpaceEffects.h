#pragma once
#include "PostProcessing/PostProcessEffect.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)

	MGF_CLASS_PTR(WorldSpaceEffects)
	class WorldSpaceEffects : public PostProcessEffect
	{
		using Super = PostProcessEffect;

	public:
		virtual ~WorldSpaceEffects();
		static WorldSpaceEffectsUPtr Create();

	public:
		virtual bool Render(RenderContext* context) override;
		virtual void Resize(int32 width, int32 height) override;

	private:
		WorldSpaceEffects();
		bool Init();

		// World-space effect program
		GraphicsProgramPtr m_worldSpaceEffectProgram { nullptr };

		// Fog effect params
		vec3 m_fogColor{ 1.0f, 1.0f, 1.0f };
		float m_fogDensity{ 0.00005f };

		// Pixelization effect params
		float m_pixelSize{ 4.0f };

		// Posterization effect params
		float m_postLevels{ 0.0f };

		// Dilation effect params
		int32 m_dilationRadius{ 0 };
	};
}

