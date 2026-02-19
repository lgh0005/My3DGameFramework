#pragma once
#include "Light.h"

// TODO : 더 이후에는 Compute shading을 통해서 최적화를 할 수 있겠지만, 그건 이후에
// 차근차근 더 공부하고 보강해보는 것으로 한다.

#pragma region FORWARD_DECLARATION
CLASS_PTR(Transform)
#pragma endregion

CLASS_PTR(DirectionalLight)
class DirectionalLight : public Light
{
	DEFINE_COMPONENT_TYPE(ComponentType::DirectionalLight)

public:
	virtual ~DirectionalLight();
	static DirectionalLightUPtr Create();
	
	const glm::vec3& GetDirection()  const		    { return m_direction; }
	void SetDirection(const glm::vec3& direction)   { m_direction = direction; }

private:
	DirectionalLight();
	glm::vec3 m_direction	{ glm::vec3(-0.2f, -1.0f, 0.3f) };
};