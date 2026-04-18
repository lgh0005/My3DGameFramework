#pragma once
#include "Sources/Resource.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GraphicsProgram)
	MGF_CLASS_PTR(GLTextureHandle)

	enum class ETextureSlot : uint8
	{
		Albedo	 = 0,
		Specular,
		Emission,
		Normal,
		Height,
		ORM,
		Max
	};

	MGF_CLASS_PTR(Material)
	class Material : public Resource
	{
	public:
		Material();
		virtual ~Material() override;
		static MaterialPtr Create(StringView matName, const GraphicsProgramPtr& program);
		virtual bool OnSyncCreate() override;

	/*========================//
	//     Material Type      //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		void Bind() const;
		void SetProgram(const GraphicsProgramPtr& program);
		GraphicsProgramPtr GetProgram() const { return m_program; }
		void SetTexture(ETextureSlot slot, const GLTextureHandlePtr& texture);
		GLTextureHandlePtr GetTexture(ETextureSlot slot) const;
		void SetHash(StringView matName) { m_materialHash = StringHash(matName); }
		StringHash GetHash() const { return m_materialHash; }

	private:
		float shininess			{ 32.0f };
		float emissionStrength	{ 1.0f };
		float heightScale		{ 1.0f };

		vec4  albedoFactor		{ 1.0f, 1.0f, 1.0f, 1.0f };
		vec3  emissiveFactor	{ 0.0f, 0.0f, 0.0f };

		float metallicFactor{ 1.0f };
		float roughnessFactor{ 1.0f };

	private:
		StringHash m_materialHash;
		GraphicsProgramPtr m_program;
		HashMap<ETextureSlot, GLTextureHandlePtr> m_textures;
	};
}