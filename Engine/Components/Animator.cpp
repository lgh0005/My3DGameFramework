#include "EnginePch.h"
#include "Animator.h"
#include "Resources/AnimChannel.h"
#include "Resources/Model.h"

AnimatorUPtr Animator::Create(ModelPtr model)
{
	auto animator = AnimatorUPtr(new Animator());
	if (!animator->Init(std::move(model))) return nullptr;
	return animator;
}

bool Animator::Init(ModelPtr model)
{
	if (!model)
	{
		SPDLOG_ERROR("Animator::Init failed: model is nullptr");
		return false;
	}

	m_currentModel = model;
	m_currentTime = 0.0f;

    // 1. Bone Count에 맞게 스키닝 행렬 버퍼 초기화
	usize boneCount = m_currentModel->GetBoneCount();
	m_finalBoneMatrices.resize(boneCount, glm::mat4(1.0f));
	m_globalJointPositions.resize(boneCount, glm::vec3(0.0f));

    // 2. 전체 노드 개수에 맞게 글로벌 행렬 버퍼 미리 확보
    usize nodeCount = m_currentModel->GetNodes().size();
    m_globalTransforms.resize(nodeCount, glm::mat4(1.0f));

	return true;
}

void Animator::PlayAnimation(AnimationPtr animation)
{
    m_currentAnimation = std::move(animation);
    m_currentTime = 0.0f;
}

/*====================================//
//   animation update logic methods   //
//====================================*/
void Animator::Update()
{
    if (!m_currentModel) return;

    // 1. 시간 업데이트
    UpdateAnimationTime();

    // 2. 뼈대 변환 행렬 계산 (순회 + 스키닝)
    UpdateBoneTransforms();

    // 3. 바운딩 박스 업데이트
    UpdateCurrentPoseLocalBounds();
}

void Animator::UpdateAnimationTime()
{
    if (m_currentAnimation)
    {
        m_currentTime += m_currentAnimation->GetTicksPerSecond() * TIME.GetDeltaTime();
        float duration = m_currentAnimation->GetDuration();
        if (duration > 0.0f) m_currentTime = fmod(m_currentTime, duration);
    }
}

void Animator::UpdateBoneTransforms()
{
    const auto& nodes = m_currentModel->GetNodes();
    
    // 만약 런타임에 모델이 바뀌거나 해서 사이즈가 다르면 재조정
    if (m_globalTransforms.size() != nodes.size())
        m_globalTransforms.resize(nodes.size());

    // 노드 선형 순회
    for (usize i = 0; i < nodes.size(); ++i)
    {
        const auto& node = nodes[i];

        // A. 기본 로컬 변환 (T-Pose 상태)
        glm::mat4 localTransform = node.localTransform;

        // B. 애니메이션 적용 (채널이 존재하면 덮어쓰기)
        if (m_currentAnimation)
        {
            // 이름으로 채널 검색 (FindBone -> FindChannel 권장)
            const AnimChannel* channel = m_currentAnimation->FindChannel(node.name);
            if (channel) localTransform = channel->GetLocalTransform(m_currentTime);
        }

        // C. 부모 행렬과 결합 (Global Transform 계산)
        // TODO: 추후 GameObject Transform 계층 구조와 통합 고려 필요
        int32 parentIndex = node.parentIndex;
        if (parentIndex == -1) m_globalTransforms[i] = localTransform;
        else m_globalTransforms[i] = m_globalTransforms[parentIndex] * localTransform;

        // D. 스키닝 행렬(FinalBoneMatrix) 계산
        // 현재 노드가 실제 뼈(Bone)로 등록되어 있는지 확인
        const auto& boneInfoMap = m_currentModel->GetBoneInfoMap();
        auto it = boneInfoMap.find(node.name);

        if (it != boneInfoMap.end())
        {
            uint32 boneID = it->second.id;
            const glm::mat4& offsetMat = it->second.offset;

            // 유효한 ID 범위인지 체크
            if (boneID < m_finalBoneMatrices.size())
            {
                // Final = Global * Offset (Shader로 보낼 행렬)
                m_finalBoneMatrices[boneID] = m_globalTransforms[i] * offsetMat;

                // 바운딩 박스 계산 등을 위해 관절 위치 저장
                m_globalJointPositions[boneID] = glm::vec3(m_globalTransforms[i][3]);
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