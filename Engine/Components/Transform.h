#pragma once
#include "Component.h"

// TODO:
// 1. 이후에 부모와 자식의 관계는 transform을 통해서 찾을 수 있도록
// 만들 필요가 있음.
// 2. 부모에 따라 자기 자신의 transform도 변경이 되어야 한다.
// 3. 부모-자식의 컴포넌트들의 Active 순회는 Transform을 통해서.
CLASS_PTR(Transform)
class Transform : public Component
{
public:
	static TransformUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Transform;
	virtual ComponentType GetType() const override { return ComponentType::Transform; }

	// 위치
	void SetPosition(const glm::vec3& position) { m_position = position; }
	const glm::vec3& GetPosition() const		{ return m_position; }
	void Translate(const glm::vec3& delta)		{ m_position += delta; }

	// 회전
	void SetRotation(const glm::vec3& eulerAnglesDegrees);
	void SetRotation(const glm::quat& rotation) { m_rotation = rotation; }
	void Rotate(const glm::vec3& eulerAnglesDegrees);
	glm::vec3 GetRotationEuler() const;
	void SetRotation(const glm::vec3& axis, float angleRadians);
	const glm::quat& GetRotationQuat() const { return m_rotation; }

	// 스케일
	void SetScale(const glm::vec3& scale) { m_scale = scale; }
	const glm::vec3& GetScale() const { return m_scale; }

	// 방향 벡터
	glm::vec3 GetForwardVector() const; // 앞쪽   (-Z)
	glm::vec3 GetUpVector() const;      // 위쪽   (+Y)
	glm::vec3 GetRightVector() const;   // 오른쪽 (+X)

	// 모델 행렬
	glm::mat4 GetModelMatrix() const;

private:
	Transform() = default;
	glm::vec3 m_position	{ 0.0f, 0.0f, 0.0f };
	glm::quat m_rotation	{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 m_scale		{ 1.0f, 1.0f, 1.0f };
};