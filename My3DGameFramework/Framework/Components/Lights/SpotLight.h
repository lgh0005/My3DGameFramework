#pragma once
#include "Light.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Transform)

	MGF_CLASS_PTR(SpotLight)
	class SpotLight : public Light
	{
		DEFINE_COMPONENT_TYPE(ComponentType::SpotLight)

	public:
		virtual ~SpotLight();
		static SpotLightUPtr Create();

		const glm::vec2 GetCutoff()				const { return m_cutoff; }
		const float GetDistance()				const { return m_distance; }
		const glm::vec3& GetDirection()			const { return m_direction; }
		const glm::vec3 GetAttenuation()        const { return Utils::GetAttenuationCoeff(m_distance); }

		void SetCutoff(const glm::vec2& cutoff) { m_cutoff = cutoff; }
		void SetDistance(float distance) { m_distance = distance; }
		void SetDirection(const glm::vec3& direction) { m_direction = direction; }

	private:
		SpotLight();

		glm::vec2 m_cutoff{ 20.0f, 5.0f };
		float m_distance{ 32.0f };
		glm::vec3 m_direction{ glm::vec3(-0.2f, -1.0f, 0.3f) };
	};
}

