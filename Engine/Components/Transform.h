#pragma once
#include "Component.h"

// TODO:
// 1. 부모-자식의 컴포넌트들의 Active 순회는 Transform을 통해서.
CLASS_PTR(Transform)
class Transform : public Component
{
public:
	static TransformUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::Transform;
	virtual ComponentType GetComponentType() const override { return ComponentType::Transform; }
	void Update();

	// 위치
	void SetPosition(const glm::vec3& position);
	const glm::vec3& GetPosition() const;
	void Translate(const glm::vec3& delta);

	// 회전
	void SetRotation(const glm::vec3& eulerAnglesDegrees);
	void SetRotation(const glm::quat& rotation);
	void SetRotation(const glm::vec3& axis, float angleRadians);
	glm::vec3 GetRotationEuler() const;
	const glm::quat& GetRotationQuat() const;
	void Rotate(const glm::vec3& eulerAnglesDegrees);

	// 스케일
	void SetScale(const glm::vec3& scale);
	const glm::vec3& GetScale() const;

	// 월드 좌표계
	glm::vec3 Transform::GetWorldPosition() const { return m_worldPosition; }
	glm::vec3 Transform::GetWorldScale() const { return m_worldScale; }
	glm::quat Transform::GetWorldRotation() const { return m_worldRotation; }

	// 방향 벡터
	glm::vec3 GetForwardVector() const; // 앞쪽   (-Z)
	glm::vec3 GetUpVector() const;      // 위쪽   (+Y)
	glm::vec3 GetRightVector() const;   // 오른쪽 (+X)

	// 월드 또는 로컬 모델 행렬
	glm::mat4 GetLocalMatrix() const;
	glm::mat4 GetWorldMatrix() const;

	// 계층 구조
	void SetParent(Transform* parent);
	Transform* GetParent() const { return m_parent; }
	const std::vector<Transform*>& GetChildren() const { return m_children; }
	GameObject* GetChildGameObjectByIndex(usize index) const;
	GameObject* GetChildGameObjectByName(const std::string& name) const;

private:
	void AddChild(Transform* child);
	void RemoveChild(Transform* child);
	void UpdateWorldTransform();
	void SetTransformDirty();

	glm::mat4 m_worldMatrix { 1.0f };
	bool m_isTransformDirty	{ true };

private:
	Transform() = default;
	glm::vec3 m_position		{ 0.0f, 0.0f, 0.0f };
	glm::quat m_rotation		{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 m_scale			{ 1.0f, 1.0f, 1.0f };

	glm::vec3 m_worldPosition	{ 0.0f, 0.0f, 0.0f };
	glm::quat m_worldRotation	{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 m_worldScale		{ 1.0f, 1.0f, 1.0f };

	Transform* m_parent		{ nullptr };
	std::vector<Transform*> m_children;
};