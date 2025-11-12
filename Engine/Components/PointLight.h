#pragma once
#include "Components/Bases/Light.h"

// TODO : 이후에는 Deffered shading을 이용해서 다중 조명들을 그릴 방법을 적용.
// 더 이후에는 Compute shading을 통해서 최적화를 할 수 있겠지만, 그건 이후에
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

	// TEMP : GameObject가 들고 있어야 할 컴포넌트
	Transform& GetTransform() { return *m_transform; }
	const Transform& GetTransform() const { return *m_transform; }

	const float GetDistance() const { return m_distance; }
	void SetDistance(float distance)			{ m_distance = distance; }

private:
	PointLight() = default;

	// TEMP : GameObject가 들고 있어야 할 컴포넌트
	bool Init();
	TransformUPtr m_transform;

	float     m_distance	{ 32.0f };
};