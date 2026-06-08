#pragma once
#include "Rendering/RenderPass.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)
	MGF_CLASS_PTR(GLUniformBuffer)
	MGF_CLASS_PTR(StaticMesh)

	MGF_CLASS_PTR(MGFSkyboxPass)
	class MGFSkyboxPass : public RenderPass
	{
	public:
		virtual ~MGFSkyboxPass();
		static MGFSkyboxPassUPtr Create();

	/*=================================//
	//   MGF3D RenderPass Custom Type  //
	//=================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual void Execute(RenderContext* context) override;
		virtual void Resize() override;

	private:
		MGFSkyboxPass();
		bool Init();

		GraphicsProgramPtr m_skyboxProgram;
		GLUniformBufferUPtr m_skyboxUBO;
		StaticMeshPtr m_cubeMesh;
	};
}