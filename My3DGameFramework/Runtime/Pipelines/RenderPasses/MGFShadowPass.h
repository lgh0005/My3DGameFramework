#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Camera)
	MGF_CLASS_PTR(GraphicsProgram)
	MGF_CLASS_PTR(GLFramebuffer2D)
	MGF_CLASS_PTR(GLTexture2DArray)
	MGF_CLASS_PTR(GLFramebufferCube)
	MGF_CLASS_PTR(GLTextureCubeArray)

	MGF_CLASS_PTR(MGFShadowPass)
	class MGFShadowPass : public RenderPass
	{
	public:
		virtual ~MGFShadowPass();
		static MGFShadowPassUPtr Create();

		/*=================================//
		//   MGF3D RenderPass Custom Type  //
		//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;

	private:
		void RenderDirectionalShadows(RenderContext* context);
		void RenderPointShadows(RenderContext* context);
		void RenderSpotShadows(RenderContext* context);
		Vector<mat4> CalculateCSMMatrices(const Camera* camera, const vec3& lightDir, vec4& outSplits);

	private:
		MGFShadowPass();
		bool Init();

		// 셰이더 프로그램 (정적/스켈레탈 분리 권장)
		GraphicsProgramPtr m_dirShadowProgram{ nullptr };
		GraphicsProgramPtr m_pointShadowProgram{ nullptr };
		GraphicsProgramPtr m_spotShadowProgram{ nullptr };

		// Directional Light (CSM) 리소스
		GLTexture2DArrayPtr m_dirShadowMapArray{ nullptr };
		GLFramebuffer2DPtr  m_dirShadowFBO{ nullptr };

		// Point Light 리소스 - Cube Array
		GLTextureCubeArrayPtr m_pointShadowMapArray{ nullptr };
		GLFramebufferCubePtr  m_pointShadowFBO{ nullptr };

		// Spot Light 리소스 - 2D Array
		GLTexture2DArrayPtr m_spotShadowMapArray{ nullptr };
		GLFramebuffer2DPtr  m_spotShadowFBO{ nullptr };
	};
}