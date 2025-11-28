#pragma once
#include "Light.h"

// TODO : 이후에는 Deffered shading을 이용해서 다중 조명들을 그릴 방법을 적용.
// 더 이후에는 Compute shading을 통해서 최적화를 할 수 있겠지만, 그건 이후에
// 차근차근 더 공부하고 보강해보는 것으로 한다.

#pragma region FORWARD_DECLARATION
CLASS_PTR(Transform)
#pragma endregion

CLASS_PTR(SpotLight)
class SpotLight : public Light
{
public:
	static SpotLightUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Light;
	virtual LightType GetLightType() const override { return LightType::Spot; }

	const glm::vec2 GetCutoff()				const { return m_cutoff; }
	const float GetDistance()				const { return m_distance; }
	const glm::vec3& GetDirection()			const { return m_direction; }
	const glm::vec3 GetAttenuation()        const { return Utils::GetAttenuationCoeff(m_distance); }

	void SetCutoff(const glm::vec2& cutoff)		  { m_cutoff = cutoff; }
	void SetDistance(float distance)			  { m_distance = distance; }
	void SetDirection(const glm::vec3& direction) { m_direction = direction; }

private:
	SpotLight() = default;

	glm::vec2 m_cutoff		{ 20.0f, 5.0f };
	float m_distance		{ 32.0f };
	glm::vec3 m_direction	{ glm::vec3(-0.2f, -1.0f, 0.3f) };
};