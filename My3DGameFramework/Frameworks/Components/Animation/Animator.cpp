#include "FrameworkPch.h"
#include "Animator.h"
#include "Managers/TypeManager.h"
#include "Assets/Model.h"
#include "Mechanics/Animation/Skeleton.h"
#include "Mechanics/Animation/AnimController.h"
#include "Mechanics/Animation/AnimState.h"
#include "Assets/Animation.h"

namespace MGF3D
{
	Animator::Animator
	(
		ObjectIDHash id, ObjectIDHash ownerID,
		const ModelPtr& model, AnimControllerUPtr controller
	)
		: Super(id, ownerID) , m_currentModel(model) , m_controller(std::move(controller))
	{
		m_controller->SetOwner(this);
		if (m_currentModel)
		{
			auto skeleton = m_currentModel->GetSkeleton();
			if (skeleton)
			{
				uint32 boneCount = skeleton->GetBoneCount();
				m_globalBoneMatrices.assign(boneCount, mat4(1.0f));
				m_finalBoneMatrices.assign(boneCount, mat4(1.0f));
			}
		}
	}
	Animator::~Animator() = default;
	Animator::Animator(Animator&& other) noexcept = default;
	Animator& Animator::operator=(Animator&& other) noexcept = default;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	int16 Animator::s_typeIndex = -1;
	const MGFType* Animator::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Component");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void Animator::Update(float deltaTime)
	{
		if (!m_currentModel || !m_controller) return;
		auto skeleton = m_currentModel->GetSkeleton();
		if (!skeleton) return;

		// 1. 애니메이션 시간 및 블렌딩 상태 업데이트
		m_controller->Update(deltaTime);

		// 2. 뼈대 전역(Root 공간) 행렬 및 스키닝 행렬 계산
		CalculateBoneMatrices();

		// 3. 바운딩 박스 계산
		UpdateLocalBounds();
	}

	void Animator::CalculateBoneMatrices()
	{
		auto skeleton = m_currentModel->GetSkeleton();
		const auto& nodes = m_currentModel->GetNodes();
		const auto& boneOffsets = skeleton->GetBoneOffsets();
		uint32 nodeCount = static_cast<uint32>(nodes.size());

		// 노드 개수만큼 글로벌 행렬을 저장할 캐시
		Vector<mat4> globalNodeMatrices;
		if (globalNodeMatrices.size() < nodeCount)
			globalNodeMatrices.resize(nodeCount);

		// 1. 노드 순서대로 계층 연산을 수행합니다.
		for (uint32 i = 0; i < nodeCount; ++i)
		{
			const auto& node = nodes[i];
			StringHash nodeHash = StringHash(node.name);

			int32 boneID = skeleton->GetBoneID(nodeHash);

			// 보조 노드든 뼈든, 애니메이션 채널이 있으면 가져와야 합니다.
			mat4 localTransform = m_controller->GetLocalMatrix(nodeHash, node.localTransform);

			// 글로벌 행렬 누적 연산
			if (node.parentIndex >= 0) globalNodeMatrices[i] = globalNodeMatrices[node.parentIndex] * localTransform;
			else globalNodeMatrices[i] = localTransform;

			// 최종 스키닝 행렬 갱신 (실제 정점들을 움직이는 뼈만 처리)
			if (boneID >= 0)
			{
				m_finalBoneMatrices[boneID] = globalNodeMatrices[i] * boneOffsets[boneID];
				m_globalBoneMatrices[boneID] = globalNodeMatrices[i];
			}
		}
	}

	void Animator::UpdateLocalBounds()
	{
		vec3 minPos(FLT_MAX);
		vec3 maxPos(-FLT_MAX);

		// 뼈의 전역 행렬에서 위치(translation)만 뽑아내어 로컬 AABB 계산
		for (const mat4& globalMat : m_globalBoneMatrices)
		{
			vec3 bonePos = vec3(globalMat[3]);
			minPos = Math::Min(minPos, bonePos);
			maxPos = Math::Max(maxPos, bonePos);
		}

		m_currentLocalAABB = RenderBounds::CreateFromMinMax(minPos, maxPos);
	}
}