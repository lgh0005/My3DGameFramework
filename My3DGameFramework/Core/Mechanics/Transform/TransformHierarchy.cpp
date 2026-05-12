#include "CorePch.h"
#include "TransformHierarchy.h"
#include "Managers/TypeManager.h"
#include "Components/Transform.h"

namespace MGF3D
{
	TransformHierarchy::TransformHierarchy(Component* owner) : Super(owner) { }
	TransformHierarchy::~TransformHierarchy()
	{
		SetParent(nullptr);
		for (Transform* child : m_children)
		{
			if (child) 
				child->SetParent(nullptr);
		}
	}
	TransformHierarchy::TransformHierarchy(TransformHierarchy&& other) noexcept = default;
	TransformHierarchy& TransformHierarchy::operator=(TransformHierarchy&& other) noexcept = default;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	int16 TransformHierarchy::s_typeIndex = -1;
	const MGFType* TransformHierarchy::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Mechanic");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	/*===============================//
	//      Hierarchy Methods        //
	//===============================*/
	Transform* TransformHierarchy::GetTransformOwner() const
	{
		return static_cast<Transform*>(GetOwner());
	}

	Transform* TransformHierarchy::GetParent() const
	{
		return m_parent;
	}

	const Vector<Transform*>& TransformHierarchy::GetChildren() const
	{
		return m_children;
	}

	void TransformHierarchy::SetParent(Transform* parent)
	{
		// 1. 이미 같은 부모를 가리키고 있다면 무시
		if (m_parent == parent) return;

		Transform* me = GetTransformOwner();
		if (!me) return;

		// 2. 기존 부모가 있었다면, 기존 부모의 자식 목록에서 나를 제거
		if (m_parent != nullptr) m_parent->RemoveChild(me);

		// 3. 부모 교체
		m_parent = parent;

		// 4. 새로운 부모가 있다면, 그 부모의 자식 목록에 나를 추가
		if (m_parent != nullptr) m_parent->AddChild(me);

		// 5. 부모가 바뀌었으므로 월드 행렬이 변해야 함 -> 더티 플래그 켜기
		me->SetTransformDirty();
	}

	void TransformHierarchy::AddChild(Transform* child)
	{
		if (!child) return;
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it == m_children.end()) m_children.push_back(child);
	}

	void TransformHierarchy::RemoveChild(Transform* child)
	{
		if (!child) return;
		auto it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end()) m_children.erase(it);
	}

	/*===============================//
	//     Calculation Methods       //
	//===============================*/
	const mat4& TransformHierarchy::GetWorldMatrix() const
	{
		if (m_isTransformDirty)
		{
			Transform* me = GetTransformOwner();
			if (me)
			{
				if (m_parent != nullptr)
				{
					// 행렬에서 역산하지 않고, 부모 성분과 직접 계산 후 캐싱
					m_worldScale = m_parent->GetWorldScale() * me->GetLocalScale();

					// 쿼터니언 곱셈으로 회전 캐싱
					m_worldRotation = m_parent->GetWorldRotation() * me->GetLocalRotation();

					// 부모 공간을 기준으로 위치 계산
					m_worldPosition = m_parent->GetWorldPosition() +
						(m_parent->GetWorldRotation() * (m_parent->GetWorldScale() * me->GetLocalPosition()));
				}
				else
				{
					// 루트 노드인 경우 로컬이 곧 월드 상태
					m_worldPosition = me->GetLocalPosition();
					m_worldRotation = me->GetLocalRotation();
					m_worldScale = me->GetLocalScale();
				}

				// 캐싱된 성분으로 최종 월드 행렬 조립
				mat4 T = glm::translate(mat4(1.0f), m_worldPosition);
				mat4 R = glm::mat4_cast(m_worldRotation);
				mat4 S = glm::scale(mat4(1.0f), m_worldScale);
				m_worldMatrix = T * R * S;

				// 자식 노드들에게 갱신 필요 알림
				for (Transform* child : m_children)
				{
					if (child) 
						child->SetTransformDirty();
				}
			}
			ClearTransformDirty();
		}

		return m_worldMatrix;
	}

	void TransformHierarchy::SetWorldMatrix(const mat4& matrix)
	{
		m_worldMatrix = matrix;
		ClearTransformDirty();

		// 강제로 행렬이 주입되었을 때는 성분 분해를 수행해야 캐시의 정합성이 유지됩니다.
		m_worldPosition = vec3(matrix[3]);

		m_worldScale = vec3
		(
			glm::length(vec3(matrix[0])),
			glm::length(vec3(matrix[1])),
			glm::length(vec3(matrix[2]))
		);

		vec3 right = glm::normalize(vec3(matrix[0]));
		vec3 up = glm::normalize(vec3(matrix[1]));
		vec3 forward = glm::normalize(vec3(matrix[2]));
		mat3 rotMat(right, up, forward);
		m_worldRotation = glm::quat_cast(rotMat);

		// 자식들에게도 갱신을 전파합니다.
		for (Transform* child : m_children)
		{
			if (child) 
				child->SetTransformDirty();
		}
	}

	vec3 TransformHierarchy::ExtractWorldPosition() const
	{
		GetWorldMatrix();
		return m_worldPosition;
	}

	vec3 TransformHierarchy::ExtractWorldScale() const
	{
		GetWorldMatrix();
		return m_worldScale;
	}

	quat TransformHierarchy::ExtractWorldRotation() const
	{
		GetWorldMatrix();
		return m_worldRotation;
	}
}