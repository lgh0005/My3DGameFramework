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

	public:

		// Fog
		vec3 GetFogColor() const noexcept
		{
			return m_fogColor;
		}

		void SetFogColor(const vec3& fogColor) noexcept
		{
			m_fogColor = fogColor;
		}

		float GetFogDensity() const noexcept
		{
			return m_fogDensity;
		}

		void SetFogDensity(float fogDensity) noexcept
		{
			m_fogDensity = std::max(0.0f, fogDensity);
		}

		// Pixelization
		float GetPixelSize() const noexcept
		{
			return m_pixelSize;
		}

		void SetPixelSize(float pixelSize) noexcept
		{
			m_pixelSize = std::max(0.0f, pixelSize);
		}

		// Posterization
		float GetPostLevels() const noexcept
		{
			return m_postLevels;
		}

		void SetPostLevels(float postLevels) noexcept
		{
			m_postLevels = std::max(0.0f, postLevels);
		}

		// Dilation
		int32 GetDilationRadius() const noexcept
		{
			return m_dilationRadius;
		}

		void SetDilationRadius(int32 dilationRadius) noexcept
		{
			m_dilationRadius = std::max<int32>(0, dilationRadius);
		}

	private:
		WorldSpaceEffects();
		bool Init();

		// World-space effect program
		GraphicsProgramPtr m_worldSpaceEffectProgram { nullptr };

		// Fog effect params
		vec3 m_fogColor{ 1.0f, 1.0f, 1.0f };
		float m_fogDensity{ 0.005f };

		// Pixelization effect params
		float m_pixelSize{ 0.0f };

		// Posterization effect params
		float m_postLevels{ 0.0f };

		// Dilation effect params
		int32 m_dilationRadius{ 0 };
	};
}

