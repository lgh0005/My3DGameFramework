#pragma once
#include "Light.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Transform)

	MGF_CLASS_PTR(PointLight)
	class PointLight : public Light
	{
		DEFINE_COMPONENT_TYPE(ComponentType::PointLight)

	public:
		virtual ~PointLight();
		static PointLightUPtr Create();

		const float GetDistance() const { return m_distance; }
		const glm::vec3 GetAttenuation() const { return Utils::GetAttenuationCoeff(m_distance); }

		void SetDistance(float distance) { m_distance = distance; }

	private:
		PointLight();
		float     m_distance{ 32.0f };
	};
}
