#include "Transform.h"
#include "EnginePch.h"
#include "Core/GameObject.h"

TransformUPtr Transform::Create()
{
	auto transform = TransformUPtr(new Transform());
	return transform;
}

void Transform::Update()
{
	if (!m_isTransformDirty) return;

	// 1. 로컬 행렬은 여기서 잠깐 계산하고
	glm::mat4 localMat = GetLocalMatrix();

	// 2. 월드 행렬(멤버)에 결과를 저장
	if (m_parent) m_worldMatrix = m_parent->GetWorldMatrix() * localMat;
	else m_worldMatrix = localMat;

	// 3. 갱신을 했으므로 더 이상 dirty하지 않음.
	m_isTransformDirty = false;
}

/*====================//
//  position methods  //
//====================*/
void Transform::SetPosition(const glm::vec3& position)
{
	m_position = position;
	SetTransformDirty();
}

const glm::vec3& Transform::GetPosition() const
{
	return m_position;
}

void Transform::Translate(const glm::vec3& delta)
{
	m_position += delta;
	SetTransformDirty();
}

/*====================//
//  rotation methods  //
//====================*/
void Transform::SetRotation(const glm::vec3& eulerAnglesDegrees)
{
	glm::vec3 eulerRadians = glm::radians(eulerAnglesDegrees);
	m_rotation = glm::quat(eulerRadians);
	SetTransformDirty();
}

void Transform::SetRotation(const glm::quat& rotation)
{
	m_rotation = rotation;
	SetTransformDirty();
}

void Transform::SetRotation(const glm::vec3& axis, float angleRadians)
{
	m_rotation = glm::angleAxis(angleRadians, glm::normalize(axis));
	SetTransformDirty();
}

void Transform::Rotate(const glm::vec3& eulerAnglesDegrees)
{
	glm::vec3 eulerRadians = glm::radians(eulerAnglesDegrees);
	glm::quat deltaRotation = glm::quat(eulerRadians);
	m_rotation = m_rotation * deltaRotation;
	m_rotation = glm::normalize(m_rotation);
	SetTransformDirty();
}

glm::vec3 Transform::GetRotationEuler() const
{
	glm::vec3 eulerRadians = glm::eulerAngles(m_rotation);
	return glm::degrees(eulerRadians);
}

const glm::quat& Transform::GetRotationQuat() const
{
	return m_rotation;
}

/*=================//
//  scale methods  //
//=================*/
void Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
	SetTransformDirty();
}

const glm::vec3& Transform::GetScale() const
{
	return m_scale;
}

/*============================//
//  direction vector methods  //
//============================*/
glm::vec3 Transform::GetForwardVector() const
{
	return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Transform::GetUpVector() const
{
	return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Transform::GetRightVector() const
{
	return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

/*==========================//
//  calculate model matrix  //
//==========================*/
glm::mat4 Transform::GetLocalMatrix() const
{
	glm::mat4 transMat = glm::translate(glm::mat4(1.0f), m_position);
	glm::mat4 rotMat = glm::toMat4(m_rotation);
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), m_scale);
	return transMat * rotMat * scaleMat;
}

glm::mat4 Transform::GetWorldMatrix() const
{
	return m_worldMatrix;
}

/*==========================//
//  hierarchical structure  //
//==========================*/
void Transform::SetParent(Transform* parent)
{
	// 1. 자기 자신을 부모로 설정하거나 이미 올바른 부모로
	// 설정되었다면 return.
	if (this == parent || m_parent == parent) return;

	// 2. 이전 부모가 남아 있는 것이 있다면 갱신 및 새 부모 설정
	if (m_parent) m_parent->RemoveChild(this);
	m_parent = parent;

	// 3. 자식 명단에 등록
	if (m_parent) m_parent->AddChild(this);

	// TODO: 부모가 바뀌었으니 월드 행렬을 다시 계산해야 함 (Dirty Flag 등)
	Update();
}

GameObject* Transform::GetChildGameObjectByIndex(usize index) const
{
	if (index >= m_children.size()) return nullptr;
	return m_children[index]->GetOwner();
}

GameObject* Transform::GetChildGameObjectByName(const std::string& name) const
{
	for (Transform* child : m_children)
	{
		GameObject* owner = child->GetOwner(); 
		if (!owner) continue;
		if (owner->GetName() == name) return owner;
	}

	return nullptr;
}

void Transform::AddChild(Transform* child)
{
	// 이미 있는지 확인하고 넣음 (중복 방지)
	auto it = std::find(m_children.begin(), m_children.end(), child);
	if (it == m_children.end()) m_children.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
	auto it = std::find(m_children.begin(), m_children.end(), child);
	if (it != m_children.end()) m_children.erase(it);
}

void Transform::SetTransformDirty()
{
	// 1. 이미 dirty 상태라면, 자식들도 이미 dirty일 테니 중복 호출 방지
	if (m_isTransformDirty) return;

	// 2. 나를 Dirty로 마킹
	m_isTransformDirty = true;

	// 3. 내 자식들도 전부 Dirty로 전파 (재귀 호출)
	for (Transform* child : m_children)
		child->SetTransformDirty();
}
