#pragma once
#include "Entities/Component.h"

namespace MGF3D
{
	MGF_CLASS_PTR(SkyCube)
	MGF_CLASS_PTR(EnvironmentMap)

	MGF_CLASS_PTR(SkyLight)
	class SkyLight : public Component
	{
		MGF_DISABLE_COPY(SkyLight)
		using Super = Component;
	
	public:
		SkyLight(ObjectIDHash id, ObjectIDHash ownerID, SkyCubeUPtr skyCube);
		virtual ~SkyLight() override;
		SkyLight(SkyLight&& other) noexcept;
		SkyLight& operator=(SkyLight&& other) noexcept;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		SkyCube* GetSkyCube() const { return m_skyCube.get(); }

		void SetEnvironmentMap(const EnvironmentMapPtr& envMap);
		EnvironmentMapPtr GetEnvironmentMap() const;

		void SetIntensity(float intensity) { m_intensity = intensity; }
		float GetIntensity() const { return m_intensity; }
		void SetTint(const vec3& tint) { m_tint = tint; }
		const vec3& GetTint() const { return m_tint; }
		void SetRotation(float rotation) { m_rotation = rotation; }
		float GetRotation() const { return m_rotation; }
		void SetMaxReflectionLod(float lod) { m_maxReflectionLod = lod; }
		float GetMaxReflectionLod() const { return m_maxReflectionLod; }

	private:
		SkyCubeUPtr m_skyCube{ nullptr };

		float m_intensity{ 1.0f };
		vec3  m_tint{ 1.0f, 1.0f, 1.0f };
		float m_rotation{ 0.0f };
		float m_maxReflectionLod{ 4.0f };
	};
}