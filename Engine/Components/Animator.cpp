#include "EnginePch.h"
#include "Animator.h"
#include "Resources/AnimChannel.h"
#include "Resources/Model.h"
#include "Resources/Skeleton.h"
#include "Resources/AnimController.h"
#include "Resources/Pose.h"

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
		SPDLOG_ERROR("Animator::Init failed: model is nullptr");
		return false;
	}

	m_currentModel = model;

    // 0. 애니메이션 컨트롤러를 move
    m_controller = std::move(controller);
    if (!m_controller)
    {
        SPDLOG_ERROR("Animator initialized without AnimController!");
        return false;
    }

    // 1. 모델로부터 스켈레톤을 얻어옴
    auto skeleton = m_currentModel->GetSkeleton();
    if (!skeleton)
    {
        SPDLOG_WARN("Animator initialized with a model having no skeleton.");
        return false;
    }

    // 2. Bone Count에 맞게 스키닝 행렬 버퍼 초기화
	usize boneCount = skeleton->GetBoneCount();
	m_finalBoneMatrices.resize(boneCount, glm::mat4(1.0f));
	m_globalJointPositions.resize(boneCount, glm::vec3(0.0f));

    // 글로벌 행렬 버퍼 빛 포즈 벡터 초기화
    const auto& nodes = m_currentModel->GetNodes();
    usize nodeCount = m_currentModel->GetNodes().size();
    m_globalTransforms.resize(nodeCount, glm::mat4(1.0f));
    m_poses.resize(nodeCount);
    for (usize i = 0; i < nodeCount; ++i)
        m_poses[i] = Pose::FromMat4(nodes[i].localTransform);

	return true;
}

/*====================================//
//   animation update logic methods   //
//====================================*/
void Animator::Update()
{
    if (!m_currentModel) return;

    // 1. 시간 업데이트
    UpdateAnimationController();

    // 2. 뼈대 변환 행렬 계산 (순회 + 스키닝)
    UpdateBoneTransforms();

    // 3. 바운딩 박스 업데이트
    UpdateCurrentPoseLocalBounds();
}

void Animator::UpdateAnimationController() 
{
    m_controller->Update(TIME.GetDeltaTime()); 
}

void Animator::UpdateBoneTransforms()
{
    const auto& nodes = m_currentModel->GetNodes();
    auto skeleton = m_currentModel->GetSkeleton();

    // 만약 런타임에 모델이 바뀌거나 해서 사이즈가 다르면 재조정
    if (m_globalTransforms.size() != nodes.size())
        m_globalTransforms.resize(nodes.size());

    // 노드 선형 순회
    for (usize i = 0; i < nodes.size(); ++i)
    {
        const auto& node = nodes[i];

        // A. 기본 포즈 가져오기 (Init에서 미리 계산해둔 캐시 사용)
        // [수정] 매 프레임 Decompose 하던 것을 m_poses[i]로 대체
        Pose defaultPose = m_poses[i];

        // B. 컨트롤러에게 최종 포즈 요청
        // [수정] m_currentAnimation 직접 접근 삭제 -> Controller가 알아서 블렌딩해서 줌
        Pose finalPose = m_controller->GetPose(node.name, defaultPose);

        // C. Pose -> Matrix 변환
        glm::mat4 localTransform = finalPose.ToMat4();

        // D. 부모 행렬과 결합 (Global Transform)
        int32 parentIndex = node.parentIndex;
        if (parentIndex == -1) m_globalTransforms[i] = localTransform;
        else m_globalTransforms[i] = m_globalTransforms[parentIndex] * localTransform;

        // E. 스키닝 행렬(FinalBoneMatrix) 계산
        if (skeleton)
        {
            // (최적화 팁: 매번 Find하지 말고, Init에서 NodeIndex -> BoneID 매핑 테이블을 만들어두면 더 빠름)
            // 일단 현재 구조 유지
            const auto& boneInfoMap = skeleton->GetBoneInfoMap();
            auto it = boneInfoMap.find(node.name);

            if (it != boneInfoMap.end())
            {
                uint32 boneID = it->second.id;
                const glm::mat4& offsetMat = it->second.offset;

                if (boneID < m_finalBoneMatrices.size())
                {
                    m_finalBoneMatrices[boneID] = m_globalTransforms[i] * offsetMat;
                    m_globalJointPositions[boneID] = glm::vec3(m_globalTransforms[i][3]);
                }
            }
        }
    }
}

void Animator::UpdateCurrentPoseLocalBounds()
{
	glm::vec3 minPos(FLT_MAX);
	glm::vec3 maxPos(-FLT_MAX);

	// 1. 순수 위치 배열을 순회합니다.
	for (const auto& jointPos : m_globalJointPositions)
	{
		// AABB 갱신 (모든 뼈 관절을 포함하는 박스)
		minPos = Utils::Min(minPos, jointPos);
		maxPos = Utils::Max(maxPos, jointPos);
	}

	// 2. 뼈대의 최대 영역 AABB를 캐시합니다.
	// Note: 이 박스는 오직 뼈 관절의 위치만을 기준으로 합니다. 
	//        메쉬의 두께는 MeshRenderer에서 합칠 겁니다.
	m_currentLocalAABB = RenderBounds::CreateFromMinMax(minPos, maxPos);
}