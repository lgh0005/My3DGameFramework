#pragma once
#include "Rendering/RenderPass.h"
#include "Uniforms/SSAOKernalUniform.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)
	MGF_CLASS_PTR(GLUniformBuffer)
	MGF_CLASS_PTR(GLFramebuffer2D)
	MGF_CLASS_PTR(GLTexture2D)

	MGF_CLASS_PTR(MGFSSAOPass)
	class MGFSSAOPass : public RenderPass
	{
	public:
		virtual ~MGFSSAOPass();
		static MGFSSAOPassUPtr Create
		(
			int32 width = WINDOW_WIDTH,
			int32 height = WINDOW_HEIGHT
		);

	/*=================================//
	//   MGF3D RenderPass Custom Type  //
	//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;

	private:
		void GenerateKernel();
		void GenerateNoiseTexture();

	private:
		MGFSSAOPass();
		bool Init(int32 width, int32 height);

		GraphicsProgramPtr m_ssaoProgram{ nullptr };
		GraphicsProgramPtr m_ssaoBlurProgram{ nullptr };
		
		SSAOKernelData     m_kernelData;
		GLUniformBufferPtr    m_ssaoKernalUBO{ nullptr };
		GLTexture2DPtr m_noiseTexture{ nullptr };

		GLFramebuffer2DPtr    m_ssaoFBO{ nullptr };
		GLFramebuffer2DPtr    m_ssaoBlurFBO{ nullptr };
	};
}