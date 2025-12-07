#pragma once
#include "Light.h"

// TODO : 더 이후에는 Compute shading을 통해서 최적화를 할 수 있겠지만, 그건 이후에
// 차근차근 더 공부하고 보강해보는 것으로 한다.

#pragma region FORWARD_DECLARATION
CLASS_PTR(Transform)
#pragma endregion

CLASS_PTR(PointLight)
class PointLight : public Light
{
public:
	static PointLightUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Light;
	virtual LightType GetLightType() const override { return LightType::Point; }

	const float GetDistance() const { return m_distance; }
	const glm::vec3 GetAttenuation() const      { return Utils::GetAttenuationCoeff(m_distance); }

	void SetDistance(float distance)			{ m_distance = distance; }

private:
	PointLight() = default;
	float     m_distance	{ 32.0f };
};