#include "EnginePch.h"
#include "Animator.h"
#include "Object/GameObject.h"
#include "Components/Transform.h"
#include "Resources/AnimChannel.h"
#include "Resources/Model.h"
#include "Resources/Skeleton.h"
#include "Resources/AnimController.h"
#include "Resources/Pose.h"

DECLARE_DEFAULTS_IMPL(Animator)

AnimatorUPtr Animator::Create(ModelPtr model, AnimControllerUPtr controller)
{
	auto animator = AnimatorUPtr(new Animator());
	if (!animator->Init(std::move(model), std::move(controller))) return nullptr;
	return animator;
}

bool Animator::Init(ModelPtr model, AnimControllerUPtr controller)
{
	if (!model)
	{
		LOG_ERROR("Animator::Init failed: model is nullptr");
		return false;
	}

	m_currentModel = model;

    // 0. 애니메이션 컨트롤러를 move
    m_controller = std::move(controller);
    if (!m_controller)
    {
        LOG_ERROR("Animator initialized without AnimController!");
        return false;
    }

    // 1. 모델로부터 스켈레톤을 얻어옴
    auto skeleton = m_currentModel->GetSkeleton();
    if (!skeleton)
    {
        LOG_WARN("Animator initialized with a model having no skeleton.");
        return false;
    }

    // 2. 버퍼 초기화 (중복 제거됨)
    usize boneCount = skeleton->GetBoneCount();
    m_finalBoneMatrices.resize(boneCount, glm::mat4(1.0f));
    m_skinningTransforms.resize(boneCount, nullptr);

	return true;
}
/*========================================//
//   animation initialize logic methods   //
//========================================*/
void Animator::BindBoneTransforms()
{
    if (!m_animationBindings.empty()) return;

    GameObject* owner = GetOwner();
    if (!owner) return;

    // 1. 모든 자식 Transform을 이름 기반 맵에 등록
    m_boneTransformMap.clear();
    BindBoneTransformsFlat(&owner->GetTransform());
    LOG_INFO("Animator Binding: Found {} transforms in hierarchy.", m_boneTransformMap.size());

    // 2. Model Node 정보를 기반으로 Animation Binding 데이터 생성
    const auto& nodes = m_currentModel->GetNodes();
    m_animationBindings.reserve(nodes.size());

    for (const auto& node : nodes)
    {
        // 이름으로 Scene Transform 찾기
        uint32 nodeHash = Utils::StrHash(node.name);
        auto it = m_boneTransformMap.find(nodeHash);
        if (it != m_boneTransformMap.end())
        {
            AnimBinding binding;

            // [최적화 1] 이름을 미리 해싱하여 저장 (Update때 해싱 안 함)
            binding.nodeNameHash = Utils::StrHash(node.name);

            binding.transform = it->second;

            // [최적화 2] 초기 행렬을 미리 Pose로 분해하여 저장 (Update때 Decompose 안 함)
            binding.defaultPose = Pose::FromMat4(node.localTransform);

            m_animationBindings.push_back(binding);
        }
    }

    // 3. Skeleton 정보를 기반으로 Skinning용 Transform 벡터 채우기
    auto skeleton = m_currentModel->GetSkeleton();
    if (skeleton)
    {
        int32 mappedCount = 0;

        // 맵에 있는 모든 Transform에 대해 스켈레톤 ID가 있는지 확인
        for (const auto& [hash, transform] : m_boneTransformMap)
        {
            // [최적화 연동] Skeleton::GetBoneID가 내부적으로 해싱하거나 문자열을 받음
            int32 boneID = skeleton->GetBoneID(hash);

            if (boneID != -1 && boneID < (int32)m_skinningTransforms.size())
            {
                m_skinningTransforms[boneID] = transform;
                mappedCount++;
            }
        }
        LOG_INFO("Animator Binding: Mapped {} / {} bones.", mappedCount, skeleton->GetBoneCount());
    }
}

void Animator::BindBoneTransformsFlat(Transform* rootTransform)
{
    if (!rootTransform) return;

    // 작업 스택 (vector를 스택처럼 사용)
    std::vector<Transform*> workStack;
    workStack.reserve(32); // 뼈대 개수만큼 미리 예약하면 좋음
    workStack.push_back(rootTransform);

    // 이름으로 매핑 (GameObject 이름 = 노드/뼈 이름)
    while (!workStack.empty())
    {
        Transform* current = workStack.back();
        workStack.pop_back();

        // 맵에 등록
        uint32 nameHash = current->GetOwner()->GetNameHash();
        m_boneTransformMap[nameHash] = current;

        // 자식들을 스택에 추가
        const auto& children = current->GetChildren();
        for (auto* child : children)
        {
            workStack.push_back(child);
        }
    }
}

/*====================================//
//   animation update logic methods   //
//====================================*/
void Animator::OnStart()
{
    BindBoneTransforms();
}

void Animator::OnUpdate()
{
    if (!m_currentModel) return;

    // 1. 시간 업데이트
    m_controller->Update(TIME.GetDeltaTime());

    // 2. 뼈대 변환 행렬 계산 (순회 + 스키닝)
    UpdateAnimationToTransforms();

    // 3. 변경된 Transform을 바탕으로 스키닝 행렬 계산
    CalculateFinalBoneMatrices();

    // 4. 바운딩 박스 업데이트
    UpdateCurrentPoseLocalBounds();
}

void Animator::UpdateAnimationToTransforms()
{
    for (const auto& binding : m_animationBindings)
    {
        // 1. 캐싱된 초기 포즈
        const Pose& defaultPose = binding.defaultPose;

        // 2. 컨트롤러에게 포즈 요청 (해시값 전달)
        Pose finalPose = m_controller->GetPose(binding.nodeNameHash, defaultPose);

        // 3. Transform 컴포넌트에 로컬 변환 적용
        binding.transform->SetLocalPosition(finalPose.position);
        binding.transform->SetLocalRotation(finalPose.rotation);
        binding.transform->SetLocalScale(finalPose.scale);
    }
}

void Animator::CalculateFinalBoneMatrices()
{
    auto skeleton = m_currentModel->GetSkeleton();
    if (!skeleton) return;

    const glm::mat4& rootInverse = GetOwner()->GetTransform().GetWorldInverseMatrix();
    for (usize i = 0; i < m_skinningTransforms.size(); ++i)
    {
        Transform* transform = m_skinningTransforms[i];
        if (transform)
        {
            glm::mat4 worldMatrix = transform->GetWorldMatrix();
            m_finalBoneMatrices[i] = (rootInverse * worldMatrix) * skeleton->GetBoneOffset(i);
        }
        else
        {
            m_finalBoneMatrices[i] = glm::mat4(1.0f);
        }
    }
}

void Animator::UpdateCurrentPoseLocalBounds()
{
    glm::vec3 minPos(FLT_MAX);
    glm::vec3 maxPos(-FLT_MAX);

    const glm::mat4& rootInverse = GetOwner()->GetTransform().GetWorldInverseMatrix();

    // 1. 순수 위치 배열을 순회합니다.
    for (Transform* transform : m_skinningTransforms)
    {
        if (transform)
        {
            // 현재 뼈의 월드 위치를 가져와서 -> 로컬(Root 기준)로 변환
            glm::vec3 worldPos = transform->GetWorldPosition();
            glm::vec3 localPos = glm::vec3(rootInverse * glm::vec4(worldPos, 1.0f));

            minPos = Utils::GlmVec3Min(minPos, localPos);
            maxPos = Utils::GlmVec3Max(maxPos, localPos);
        }
    }

    // 2. 뼈대의 최대 영역 AABB를 캐시합니다.
    // Note: 이 박스는 오직 뼈 관절의 위치만을 기준으로 합니다. 
    //        메쉬의 두께는 MeshRenderer에서 합칠 겁니다.
    m_currentLocalAABB = RenderBounds::CreateFromMinMax(minPos, maxPos);
}
