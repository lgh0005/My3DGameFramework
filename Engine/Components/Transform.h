#pragma once
#include "Object/Component.h"

// TODO:
// 1. 부모-자식의 컴포넌트들의 Active 순회는 Transform을 통해서.
// 2. transform을 통해서 계층적 게임 오브젝트에 대해서 root를 찾는 방식 추가 필요.
CLASS_PTR(Transform)
class Transform : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::Transform)

public:
	virtual ~Transform();
	static TransformUPtr Create();

/*====================================================//
//                 Transformation (Local)             //
//====================================================*/
public:
	// 위치
	void SetPosition(const glm::vec3& position);
	void SetLocalPosition(const glm::vec3& position) { SetPosition(position); }
	const glm::vec3& GetLocalPosition() const { return m_position; }
	void Translate(const glm::vec3& delta);

	// 회전
	void SetRotation(const glm::vec3& eulerAnglesDegrees);
	void SetRotation(const glm::quat& rotation);
	void SetLocalRotation(const glm::quat& rotation) { SetRotation(rotation); }
	void SetRotation(const glm::vec3& axis, float angleRadians);
	void Rotate(const glm::vec3& eulerAnglesDegrees);
	const glm::quat& GetLocalRotation() const { return m_rotation; }
	glm::vec3 GetRotationEuler() const;

	// 스케일
	void SetScale(const glm::vec3& scale);
	void SetLocalScale(const glm::vec3& scale) { SetScale(scale); }
	const glm::vec3& GetLocalScale() const { return m_scale; }

/*====================================================//
//                 Transformation (World)             //
//====================================================*/
public:
	// 월드 좌표계
	void SetWorldPosition(const glm::vec3& position);
	void SetWorldRotation(const glm::quat& rotation);
	void SetWorldRotation(const glm::vec3& eulerAnglesDegrees);
	void SetWorldScale(const glm::vec3& scale);

	glm::vec3 Transform::GetWorldPosition() const;
	glm::vec3 Transform::GetWorldScale()	const;
	glm::quat Transform::GetWorldRotation() const;

	// 방향 벡터
	glm::vec3 GetForwardVector() const; // 앞쪽   (-Z)
	glm::vec3 GetUpVector() const;      // 위쪽   (+Y)
	glm::vec3 GetRightVector() const;   // 오른쪽 (+X)

/*====================================================//
//                 Matrix Calculation                 //
//====================================================*/
public:
	// 월드 또는 로컬 모델 행렬
	void SetLocalMatrix(const glm::mat4& matrix);
	glm::mat4 GetLocalMatrix() const;
	glm::mat4 GetWorldMatrix() const;
	glm::mat4 GetWorldInverseMatrix() const;

/*====================================================//
//                 Hierarchy System                   //
//====================================================*/
public:
	void SetParent(Transform* parent);
	Transform* GetParent() const { return m_parent; }
	const std::vector<Transform*>& GetChildren() const { return m_children; }
	
	// TODO : 이게 역할을 생각하자면 GameObject를 반환하는 것 보다는
	// Transform을 반환하는게 더 맞을 수도 있음.
	GameObject* GetRoot() const;
	GameObject* GetChildGameObjectByIndex(usize index) const;
	GameObject* GetChildGameObjectByName(const std::string& name) const;

private:
	void AddChild(Transform* child);
	void RemoveChild(Transform* child);
	void UpdateTransform() const;
	void UpdateWorldInverseTransform() const;
	void SetTransformDirty();

private:
	Transform();
	glm::vec3 m_position				{ 0.0f, 0.0f, 0.0f };
	glm::quat m_rotation				{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 m_scale					{ 1.0f, 1.0f, 1.0f };

	Transform* m_parent					{ nullptr };
	std::vector<Transform*> m_children;

	mutable glm::mat4 m_worldMatrix		{ 1.0f };
	mutable bool m_isTransformDirty		{ true };
	mutable glm::vec3 m_worldPosition	{ 0.0f, 0.0f, 0.0f };
	mutable glm::quat m_worldRotation	{ 1.0f, 0.0f, 0.0f, 0.0f };
	mutable glm::vec3 m_worldScale		{ 1.0f, 1.0f, 1.0f };
	mutable glm::mat4 m_worldInverseMatrix{ 1.0f };
	mutable bool m_isWorldInverseDirty{ true };
};