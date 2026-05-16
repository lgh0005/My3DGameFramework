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
		virtual bool Render
		(
			RenderContext* context,
			GLFramebuffer2D* srcFBO,
			GLFramebuffer2D* dstFBO
		) override;
		virtual void Resize(int32 width, int32 height) override;

	private:
		WorldSpaceEffects();
		bool Init();

		// Fog effect params
		vec3 m_fogColor{ 1.0f, 1.0f, 1.0f };
		float m_fogDensity{ 0.0f };

		// Pixelization effect params
		float m_pixelSize{ 0.0f };

		// Posterization effect params
		float m_postLevels{ 0.0f };

		// Dilation effect params
		int32 m_dilationRadius{ 0 };
	};
}





//#pragma once
//#include "Pipelines/PostProcessing/PostProcessEffect.h"
//
//#pragma region FORWARD_DECLARATION
//CLASS_PTR(RenderContext)
//CLASS_PTR(Program)
//CLASS_PTR(PostProcessFramebuffer)
//#pragma endregion
//
//CLASS_PTR(WorldSpaceEffects)
//class WorldSpaceEffects : public PostProcessEffect
//{
//public:
//	virtual ~WorldSpaceEffects();
//	static WorldSpaceEffectsUPtr Create
//	(
//		int32 priority,
//		int32 width = WINDOW_WIDTH,
//		int32 height = WINDOW_HEIGHT
//	);
//
//	virtual bool Render
//	(
//		RenderContext* context,
//		Framebuffer* src,
//		Framebuffer* dst,
//		ScreenMesh* scrn
//	) override;
//
//private:
//	WorldSpaceEffects();
//	bool Init(int32 priority, int32 width, int32 height);
//
//	ProgramPtr m_worldSpaceEffectProgram	{ nullptr };
//
//	// Fog
//	glm::vec3 m_fogColor{ 1.0f, 0.6f, 0.7f };
//	float m_fogDensity{ 0.0f };
//
//	// Pixelization
//	float m_pixelSize{ 0.0f };
//
//	// Posterization
//	float m_postLevels{ 0.0f };
//
//	// Dilation
//	int32 m_dilationRadius{ 0 };
//};