#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
#pragma endregion

CLASS_PTR(RaycasterHit)
class RaycastHit
{
public:
	RaycastHit();
	~RaycastHit();

	void Reset();
	void SetHitResult
	(
		GameObject* obj,
		const glm::vec3& point,
		const glm::vec3& normal,
		float dist
	);

	bool				HasHit() const { return m_isHit; }
	GameObject* GetGameObject() const { return m_gameObject; }
	const glm::vec3& GetPoint() const { return m_point; }
	const glm::vec3& GetNormal() const { return m_normal; }
	float				GetDistance() const { return m_distance; }

private:
	GameObject* m_gameObject = nullptr;		// 충돌한 오브젝트
	glm::vec3	m_point = glm::vec3(0.f);	// 월드 충돌 좌표
	glm::vec3	m_normal = glm::vec3(0.f);	// 월드 표면 법선
	float		m_distance = 0.f;			// 거리
	bool		m_isHit = false;			// 충돌 성공 여부
};