#pragma once
#include "Sources/NamedResource.h"
#include "Uniforms/MaterialUniform.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Image)
	MGF_CLASS_PTR(GLUniformBuffer)

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
	class Material : public NamedResource
	{
		using Super = NamedResource;

	public:
		virtual ~Material() override;
		static MaterialPtr Create(StringView matName);
		virtual bool OnSyncCreate() override;

	/*========================//
	//     Material Type      //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		void Bind() const;
		void SetTexture(ETextureSlot slot, const ImagePtr& image);
		ImagePtr GetTexture(ETextureSlot slot) const;

	/*==================================//
	//       Material Parameters        //
	//==================================*/
	public:
		// Setters
		void SetAlbedoFactor(const vec4& factor) { albedoFactor = factor; }
		void SetEmissionStrength(float strength) { emissionStrength = strength; }
		void SetEmissiveFactor(const vec3& factor) { emissiveFactor = factor; }
		void SetShininess(float value) { shininess = value; }
		void SetHeightScale(float scale) { heightScale = scale; }
		void SetMetallicFactor(float factor) { metallicFactor = factor; }
		void SetRoughnessFactor(float factor) { roughnessFactor = factor; }

		// Getters
		const vec4& GetAlbedoFactor() const { return albedoFactor; }
		float       GetEmissionStrength() const { return emissionStrength; }
		const vec3& GetEmissiveFactor() const { return emissiveFactor; }
		float       GetShininess() const { return shininess; }
		float       GetHeightScale() const { return heightScale; }
		float       GetMetallicFactor() const { return metallicFactor; }
		float       GetRoughnessFactor() const { return roughnessFactor; }

	private:
		vec4  albedoFactor{ 1.0f, 1.0f, 1.0f, 1.0f };
		float emissionStrength{ 1.0f };
		vec3  emissiveFactor{ 0.0f, 0.0f, 0.0f };
		float shininess			{ 32.0f };
		float heightScale		{ 1.0f };
		float metallicFactor{ 1.0f };
		float roughnessFactor{ 1.0f };

	private:
		Material(StringView name);

		GLUniformBufferUPtr m_materialBuffer;
		Array<ImagePtr, static_cast<usize>(ETextureSlot::Max)> m_images;
	};
}