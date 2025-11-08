#pragma once

// TODO : 이후에는 Deffered shading을 이용해서 다중 조명들을 그릴 방법을 적용.
// 더 이후에는 Compute shading을 통해서 최적화를 할 수 있겠지만, 그건 이후에
// 차근차근 더 공부하고 보강해보는 것으로 한다.

#pragma region FORWARD_DECLARATION
CLASS_PTR(Transform)
#pragma endregion

CLASS_PTR(PointLight)
class PointLight
{
public:
	static PointLightUPtr Create();

	// TODO : 이후에는 GameObject가 들고 있는 Transform을 통해서
	// 접근하여 가져와야 함. 현재는 조명의 위치/방향을 조작할 때 사용.
	Transform& GetTransform() { return *m_transform; }
	const Transform& GetTransform() const { return *m_transform; }

	const float      GetDistance() const { return m_distance; }
	const glm::vec3& GetAmbient()  const { return m_ambient; }
	const glm::vec3& GetDiffuse()  const { return m_diffuse; }
	const glm::vec3& GetSpecular() const { return m_specular; }

	void SetDistance(float distance)			{ m_distance = distance; }
	void SetAmbient(const glm::vec3& ambient)	{ m_ambient = ambient; }
	void SetDiffuse(const glm::vec3& diffuse)	{ m_diffuse = diffuse; }
	void SetSpecular(const glm::vec3& specular) { m_specular = specular; }

private:
	PointLight() = default;
	bool Init();

	TransformUPtr m_transform;

	float     m_distance	{ 32.0f };
	glm::vec3 m_ambient		{ glm::vec3(0.1f, 0.1f, 0.1f) };
	glm::vec3 m_diffuse		{ glm::vec3(0.5f, 0.5f, 0.5f) };
	glm::vec3 m_specular	{ glm::vec3(1.0f, 1.0f, 1.0f) };
};