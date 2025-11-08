#pragma once

// TODO : 이후에는 Deffered shading을 이용해서 다중 조명들을 그릴 방법을 적용.
// 더 이후에는 Compute shading을 통해서 최적화를 할 수 있겠지만, 그건 이후에
// 차근차근 더 공부하고 보강해보는 것으로 한다.

#pragma region FORWARD_DECLARATION
CLASS_PTR(Transform)
#pragma endregion

CLASS_PTR(DirectionalLight)
class DirectionalLight
{
public:
	static DirectionalLightUPtr Create();

	const glm::vec3& GetDirection() const		  { return m_direction; }
	const glm::vec3& GetAmbient() const			  { return m_ambient; }
	const glm::vec3& GetDiffuse() const		      { return m_diffuse; }
	const glm::vec3& GetSpecular() const		  { return m_specular; }

	void SetDirection(const glm::vec3& direction) { m_direction = direction; }
	void SetAmbient(const glm::vec3& ambient)	  { m_ambient = ambient; }
	void SetDiffuse(const glm::vec3& diffuse)	  { m_diffuse = diffuse; }
	void SetSpecular(const glm::vec3& specular)   { m_specular = specular; }

private:
	DirectionalLight() = default;

	glm::vec3 m_direction	{ glm::vec3(-0.2f, -1.0f, 0.3f) };
	glm::vec3 m_ambient		{ glm::vec3(0.1f, 0.1f, 0.1f) };
	glm::vec3 m_diffuse		{ glm::vec3(0.5f, 0.5f, 0.5f) };
	glm::vec3 m_specular	{ glm::vec3(1.0f, 1.0f, 1.0f) };

};