#include "Transform.h"
#include "EnginePch.h"
#include "Scene/GameObject.h"

DECLARE_DEFAULTS_IMPL(Transform)

TransformUPtr Transform::Create()
{
	auto transform = TransformUPtr(new Transform());
	return transform;
}

/*====================================================//
//                 Core Logic (Lazy Update)           //
//====================================================*/
void Transform::UpdateTransform() const
{
	// 1. 부모가 있다면 부모부터 최신 상태인지 확인
	if (m_parent) m_parent->GetWorldMatrix();

	// 2. 로컬 행렬 계산 (SRT)
	glm::mat4 localMat = GetLocalMatrix();

	// 3. 월드 행렬 계산
	if (m_parent) m_worldMatrix = m_parent->GetWorldMatrix() * localMat;
	else m_worldMatrix = localMat;

	// 4. 월드 속성 캐싱 (Position, Rotation, Scale 분해)
	m_worldPosition = glm::vec3(m_worldMatrix[3]);
	m_worldScale.x = glm::length(glm::vec3(m_worldMatrix[0]));
	m_worldScale.y = glm::length(glm::vec3(m_worldMatrix[1]));
	m_worldScale.z = glm::length(glm::vec3(m_worldMatrix[2]));

	if (m_worldScale.x <= glm::epsilon<float>() ||
		m_worldScale.y <= glm::epsilon<float>() ||
		m_worldScale.z <= glm::epsilon<float>())
	{
		m_worldRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		glm::mat3 rotationMat;
		rotationMat[0] = glm::vec3(m_worldMatrix[0]) / m_worldScale.x;
		rotationMat[1] = glm::vec3(m_worldMatrix[1]) / m_worldScale.y;
		rotationMat[2] = glm::vec3(m_worldMatrix[2]) / m_worldScale.z;
		m_worldRotation = glm::normalize(glm::quat_cast(rotationMat));
	}

	// 5. 갱신 완료 -> Dirty 해제
	m_isTransformDirty = false;
}

void Transform::UpdateWorldInverseTransform() const
{
	m_worldInverseMatrix = glm::affineInverse(m_worldMatrix);
	m_isWorldInverseDirty = false;
}

void Transform::SetTransformDirty()
{
	// 이미 Dirty라면 자식들도 이미 Dirty일 것이므로 패스 (중복 호출 방지)
	if (m_isTransformDirty) return;

	m_isTransformDirty = true;
	m_isWorldInverseDirty = true;

	// 자식들도 다 Dirty로 만듦 (부모가 움직이면 자식도 월드 좌표가 바뀌니까)
	for (Transform* child : m_children) child->SetTransformDirty();
}

/*====================================================//
//                 Transformation (Local)             //
//====================================================*/
void Transform::SetPosition(const glm::vec3& position)
{
	m_position = position;
	SetTransformDirty();
}

void Transform::Translate(const glm::vec3& delta)
{
	m_position += delta;
	SetTransformDirty();
}

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

void Transform::Rotate(const glm::vec3& eulerAnglesDegrees)
{
	glm::vec3 eulerRadians = glm::radians(eulerAnglesDegrees);
	glm::quat deltaRotation = glm::quat(eulerRadians);
	m_rotation = m_rotation * deltaRotation;
	m_rotation = glm::normalize(m_rotation);
	SetTransformDirty();
}

void Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
	SetTransformDirty();
}

glm::vec3 Transform::GetRotationEuler() const
{
	glm::vec3 eulerRadians = glm::eulerAngles(m_rotation);
	return glm::degrees(eulerRadians);
}

/*====================================================//
//                 Transformation (World)             //
//====================================================*/
void Transform::SetWorldPosition(const glm::vec3& position)
{
	// 1. 부모가 없다면 월드 좌표 == 로컬 좌표
	if (m_parent == nullptr)
	{
		SetPosition(position);
		return;
	}

	// 2. 부모의 월드 역행렬을 구함
	const glm::mat4& parentInverseMat = m_parent->GetWorldInverseMatrix();

	// 3. 월드 좌표를 부모 공간(Local)으로 변환
	glm::vec4 localPos4 = parentInverseMat * glm::vec4(position, 1.0f);

	// 4. 로컬 좌표 설정 (이 안에서 SetTransformDirty가 호출됨)
	SetPosition(glm::vec3(localPos4));
}

void Transform::SetWorldRotation(const glm::quat& rotation)
{
	// 1. 부모가 없다면 월드 회전 == 로컬 회전
	if (m_parent == nullptr)
	{
		SetRotation(rotation);
		return;
	}

	// 2. 부모의 월드 회전을 가져옴
	glm::quat parentRotation = m_parent->GetWorldRotation();

	// 3. 로컬 회전 계산: (부모회전^-1 * 월드회전)
	glm::quat localRotation = glm::conjugate(parentRotation) * rotation;

	// 4. 로컬 회전 설정
	SetRotation(glm::normalize(localRotation));
}

void Transform::SetWorldRotation(const glm::vec3& eulerAnglesDegrees)
{
	SetWorldRotation(glm::quat(glm::radians(eulerAnglesDegrees)));
}

void Transform::SetWorldScale(const glm::vec3& scale)
{
	// 1. 부모가 없다면 월드 스케일 == 로컬 스케일
	if (m_parent == nullptr)
	{
		SetScale(scale);
		return;
	}

	// 2. 부모의 월드 스케일을 가져옴
	glm::vec3 parentScale = m_parent->GetWorldScale();
	glm::vec3 localScale = scale;

	// 3. 부모 스케일로 나누어 로컬 스케일 역산
	if (glm::abs(parentScale.x) > glm::epsilon<float>()) localScale.x /= parentScale.x;
	else localScale.x = 0.0f;

	if (glm::abs(parentScale.y) > glm::epsilon<float>()) localScale.y /= parentScale.y;
	else localScale.y = 0.0f;

	if (glm::abs(parentScale.z) > glm::epsilon<float>()) localScale.z /= parentScale.z;
	else localScale.z = 0.0f;

	// 4. 로컬 스케일 설정
	SetScale(localScale);
}

glm::mat4 Transform::GetWorldMatrix() const
{
	if (m_isTransformDirty) UpdateTransform();
	return m_worldMatrix;
}

glm::mat4 Transform::GetWorldInverseMatrix() const
{
	if (m_isTransformDirty) UpdateTransform();
	if (m_isWorldInverseDirty) UpdateWorldInverseTransform();
	return m_worldInverseMatrix;
}

glm::vec3 Transform::GetWorldPosition() const
{
	if (m_isTransformDirty) UpdateTransform();
	return m_worldPosition;
}

glm::vec3 Transform::GetWorldScale() const
{
	if (m_isTransformDirty) UpdateTransform();
	return m_worldScale;
}

glm::quat Transform::GetWorldRotation() const
{
	if (m_isTransformDirty) UpdateTransform();
	return m_worldRotation;
}

glm::vec3 Transform::GetForwardVector() const
{
	glm::mat4 world = GetWorldMatrix();
	return glm::normalize(glm::vec3(world[2])) * -1.0f;
}

glm::vec3 Transform::GetUpVector() const
{
	glm::mat4 world = GetWorldMatrix();
	return glm::normalize(glm::vec3(world[1]));
}

glm::vec3 Transform::GetRightVector() const
{
	glm::mat4 world = GetWorldMatrix();
	return glm::normalize(glm::vec3(world[0]));
}

/*====================================================//
//                 Matrix Calculation                 //
//====================================================*/
void Transform::SetLocalMatrix(const glm::mat4& matrix)
{
	glm::vec3 scale, translation, skew;
	glm::vec4 perspective;
	glm::quat rotation;

	if (glm::decompose(matrix, scale, rotation, translation, skew, perspective))
	{
		m_position = translation;
		m_rotation = rotation;
		m_scale = scale;
		SetTransformDirty();
	}
	else
	{
		LOG_WARN("Failed to decompose matrix in Transform::SetLocalMatrix");
	}
}

glm::mat4 Transform::GetLocalMatrix() const
{
	glm::mat4 transMat = glm::translate(glm::mat4(1.0f), m_position);
	glm::mat4 rotMat = glm::toMat4(m_rotation);
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), m_scale);
	return transMat * rotMat * scaleMat;
}

/*====================================================//
//                 Hierarchy System                   //
//====================================================*/
void Transform::SetParent(Transform* parent)
{
	// 1. 자기 자신이나, 이미 같은 부모라면 무시
	if (this == parent || m_parent == parent) return;

	// 2. 기존 부모와의 연결 끊기
	if (m_parent) m_parent->RemoveChild(this);

	// 3. 새 부모 설정
	m_parent = parent;

	// 4. 새 부모의 자식 리스트에 등록
	if (m_parent) m_parent->AddChild(this);

	// 5. 부모가 바뀌었으므로 월드 행렬을 다시 계산해야 함 (Dirty 마킹)
	SetTransformDirty();
}

void Transform::AddChild(Transform* child)
{
	auto it = std::find(m_children.begin(), m_children.end(), child);
	if (it == m_children.end()) m_children.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
	auto it = std::find(m_children.begin(), m_children.end(), child);
	if (it != m_children.end()) m_children.erase(it);
}

GameObject* Transform::GetRoot() const
{
	// 2. 부모를 타고 올라감
	const Transform* current = this;
	while (current->GetParent() != nullptr) current = current->GetParent();
	return current->GetOwner();
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
