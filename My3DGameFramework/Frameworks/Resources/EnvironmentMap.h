#pragma once
#include "Sources/NamedResource.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Image)
	MGF_CLASS_PTR(GLTexture2D)
	MGF_CLASS_PTR(GLTextureCube)
	MGF_CLASS_PTR(GraphicsProgram)
	MGF_CLASS_PTR(GLUniformBuffer)
	MGF_CLASS_PTR(GLFramebuffer2D)

	MGF_CLASS_PTR(EnvironmentMap)
	class EnvironmentMap : public NamedResource
	{
		using Super = NamedResource;

	public:
		EnvironmentMap();
		virtual ~EnvironmentMap();
		static EnvironmentMapPtr Create(StringView mapName);
		virtual bool OnSyncCreate() override;

	/*==============================//
	//       Resource Type          //
	//==============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		void SetCubeTexture(const ImagePtr& image);
		GLTextureCubePtr GetSkybox() const { return m_skybox; }
		GLTextureCubePtr GetIrradiance() const { return m_irradiance; }
		GLTextureCubePtr GetPrefiltered() const { return m_prefiltered; }
		GLTexture2DPtr   GetBrdfLUT() const { return m_brdf; }

	private:
		void BakeSkybox(uint32 fbo, uint32 cubeVAO, usize indexCount);
		void BakeIrradiance(uint32 fbo, uint32 cubeVAO, usize indexCount);
		void BakePrefiltered(uint32 fbo, const GLUniformBufferUPtr& ubo, uint32 cubeVAO, usize indexCount);
		void BakeBRDF(uint32 fbo, uint32 screenVAO, usize indexCount);

	private:
		EnvironmentMap(StringView mapName);
		bool Init(StringView mapName);

		ImagePtr m_environmentCubeImage;

		GLTextureCubePtr m_skybox		{ nullptr };
		GLTextureCubePtr m_irradiance	{ nullptr };
		GLTextureCubePtr m_prefiltered	{ nullptr };
		GLTexture2DPtr   m_brdf			{ nullptr };

		GraphicsProgramPtr m_skyboxProgram;
		GraphicsProgramPtr m_irradianceProgram;
		GraphicsProgramPtr m_prefilterProgram;
		GraphicsProgramPtr m_brdfProgram;
	};
}
