#include "Transform.h"
#include "EnginePch.h"
#include "Object/GameObject.h"

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
	// 1. 나부터 위로 올라가며 Dirty한 조상들을 스택에 쌓음
	std::vector<const Transform*> updateStack;
	updateStack.reserve(16);

	const Transform* curr = this;
	while (curr && curr->m_isTransformDirty)
	{
		updateStack.push_back(curr);
		curr = curr->m_parent;
	}

	// 2. 위에서부터(Root -> 나) 내려오면서 순서대로 갱신
	while (!updateStack.empty())
	{
		const Transform* target = updateStack.back();
		updateStack.pop_back();
		target->UpdateTransformInternal();
	}
}

void Transform::UpdateTransformInternal() const
{
	// 1. 로컬 행렬 계산 (SRT)
	glm::mat4 localMat = GetLocalMatrix();

	// 2. 월드 행렬 계산
	if (m_parent) m_worldMatrix = m_parent->m_worldMatrix * localMat;
	else m_worldMatrix = localMat;

	// 3. 월드 속성 캐싱 (Position, Rotation, Scale 분해)
	m_worldPosition = glm::vec3(m_worldMatrix[3]);

	// 위치
	glm::vec3 axisX = glm::vec3(m_worldMatrix[0]);
	glm::vec3 axisY = glm::vec3(m_worldMatrix[1]);
	glm::vec3 axisZ = glm::vec3(m_worldMatrix[2]);

	// 스케일
	m_worldScale.x = glm::length(glm::vec3(m_worldMatrix[0]));
	m_worldScale.y = glm::length(glm::vec3(m_worldMatrix[1]));
	m_worldScale.z = glm::length(glm::vec3(m_worldMatrix[2]));

	// 회전
	if (Utils::HasLength(axisX) &&
		Utils::HasLength(axisY) &&
		Utils::HasLength(axisZ))
	{
		glm::mat3 rotationMat;
		rotationMat[0] = glm::vec3(m_worldMatrix[0]) / m_worldScale.x;
		rotationMat[1] = glm::vec3(m_worldMatrix[1]) / m_worldScale.y;
		rotationMat[2] = glm::vec3(m_worldMatrix[2]) / m_worldScale.z;
		m_worldRotation = glm::normalize(glm::quat_cast(rotationMat));
	}
	else
	{
		m_worldRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	}

	// 4. 갱신 완료 -> Dirty 해제
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

	// 작업 스택
	std::vector<Transform*> workStack;
	workStack.reserve(16);
	workStack.push_back(this);

	// DFS로 Dirty 마킹
	while (!workStack.empty())
	{
		// 1. 스택에서 하나 꺼냄 (LIFO)
		Transform* current = workStack.back();
		workStack.pop_back();

		// 2. Dirty 마킹
		current->m_isTransformDirty = true;
		current->m_isWorldInverseDirty = true;

		// 3. 자식들을 스택에 추가
		for (Transform* child : current->m_children)
		{
			// 이미 Dirty 상태인 자식은 굳이 스택에 넣을 필요 없음 (가지치기)
			if (!child->m_isTransformDirty)
				workStack.push_back(child);
		}
	}
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

void Transform::UpdatePrevWorldMatrix() const
{
	if (m_isTransformDirty) UpdateTransform();
	m_prevWorldMatrix = m_worldMatrix;
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

	// 2. 순환 참조 방지
	if (IsDescendant(parent))
	{
		LOG_ERROR("SetParent Failed: Cannot make a descendant the parent (Cycle detected).");
		return;
	}

	// 2. 기존 부모와의 연결 끊기
	if (m_parent) m_parent->RemoveChild(this);

	// 3. 새 부모 설정
	m_parent = parent;

	// 4. 새 부모의 자식 리스트에 등록
	if (m_parent) m_parent->AddChild(this);

	// 5. 부모가 바뀌었으므로 월드 행렬을 다시 계산해야 함 (Dirty 마킹)
	SetTransformDirty();
}

bool Transform::IsDescendant(Transform* transform)
{
	if (!transform) return false;

	std::vector<Transform*> workStack;
	workStack.reserve(16);
	for (auto* child : m_children) workStack.push_back(child);

	while (!workStack.empty())
	{
		Transform* current = workStack.back();
		workStack.pop_back();
		if (current == transform) return true;

		for (auto* child : current->m_children) 
			workStack.push_back(child);
	}

	return false;
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
	// 부모를 타고 올라감
	const Transform* current = this;
	while (current->GetParent() != nullptr) current = current->GetParent();
	return current->GetOwner();
}

GameObject* Transform::GetChildGameObjectByIndex(usize index) const
{
	if (index >= m_children.size()) return nullptr;
	return m_children[index]->GetOwner();
}

GameObject* Transform::GetChildGameObjectByNameRecursive(const std::string& name) const
{
	if (name.empty()) return nullptr;
	return GetChildGameObjectByNameRecursive(Utils::StrHash(name));
}

GameObject* Transform::GetChildGameObjectByNameRecursive(uint32 targetHash) const
{
	if (m_children.empty()) return nullptr;

	// 작업 스택 (DFS)
	std::vector<Transform*> workStack;
	workStack.reserve(16);

	// 순서 보장을 위해 역순으로 스택에 넣음 (0번부터 탐색되도록)
	for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
		workStack.push_back(*it);

	while (!workStack.empty())
	{
		Transform* current = workStack.back();
		workStack.pop_back();

		GameObject* owner = current->GetOwner();

		// [Current Code] GameObject에 해시 캐싱이 없다면:
		if (owner && owner->GetNameHash() == targetHash) return owner;

		// 자식들을 스택에 추가
		for (auto it = current->m_children.rbegin(); it != current->m_children.rend(); ++it)
			workStack.push_back(*it);
	}

	return nullptr;
}
