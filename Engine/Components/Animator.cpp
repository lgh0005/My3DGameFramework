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
    if (m_boneTransformMap.empty())
    {
        GameObject* owner = GetOwner();
        if (!owner) return;

        // 1. 재귀적으로 모든 자식 Transform을 맵에 등록
        RecursiveBindBoneTransforms(&owner->GetTransform());

        LOG_INFO("Animator Binding: Found {} transforms in hierarchy.", m_boneTransformMap.size());

        // 2. Skeleton 정보를 기반으로 Skinning용 Transform 벡터 채우기
        auto skeleton = m_currentModel->GetSkeleton();
        if (skeleton)
        {
            int32 mappedCount = 0;
            const auto& boneInfoMap = skeleton->GetBoneInfoMap();
            for (const auto& [name, info] : boneInfoMap)
            {
                if (m_boneTransformMap.count(name))
                {
                    // BoneID 위치에 Transform 포인터 저장
                    if (info.id < m_skinningTransforms.size())
                    {
                        m_skinningTransforms[info.id] = m_boneTransformMap[name];
                        mappedCount++;
                    }
                }
            }
            LOG_INFO("Animator Binding: Mapped {} / {} bones.", mappedCount, skeleton->GetBoneCount());
        }
    }
}

void Animator::RecursiveBindBoneTransforms(Transform* nodeTransform)
{
    if (!nodeTransform) return;

    // 이름으로 매핑 (GameObject 이름 = 노드/뼈 이름)
    std::string name = nodeTransform->GetOwner()->GetName();
    m_boneTransformMap[name] = nodeTransform;

    for (auto* child : nodeTransform->GetChildren())
        RecursiveBindBoneTransforms(child);
}

/*====================================//
//   animation update logic methods   //
//====================================*/
void Animator::OnUpdate()
{
    if (!m_currentModel) return;

    // 0. 바인딩이 안 되어 있다면 수행 (Lazy Init)
    // TODO : 이 또한 Component 의 생명주기에 맞춰서 코드를 적을 필요가 있음
    if (m_boneTransformMap.empty()) BindBoneTransforms();

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
    const auto& nodes = m_currentModel->GetNodes();

    // 모든 노드를 순회하며 애니메이션 포즈 적용
    for (const auto& node : nodes)
    {
        // 1. 이 노드 이름에 해당하는 Transform이 있는지 확인
        auto it = m_boneTransformMap.find(node.name);
        if (it == m_boneTransformMap.end()) continue;

        Transform* transform = it->second;

        // 2. 초기 포즈(T-Pose) 가져오기
        Pose defaultPose = Pose::FromMat4(node.localTransform);

        // 3. 컨트롤러에게 블렌딩된 최종 로컬 포즈 요청
        Pose finalPose = m_controller->GetPose(node.name, defaultPose);

        // 4. [핵심] Transform 컴포넌트 직접 수정!
        // Matrix 분해 비용 없이 Pose(TRS) 데이터를 바로 꽂아넣음
        transform->SetPosition(finalPose.position);
        transform->SetRotation(finalPose.rotation);
        transform->SetScale(finalPose.scale);
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
