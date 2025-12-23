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

	usize boneCount = m_currentModel->GetBoneCount();
	m_finalBoneMatrices.resize(boneCount, glm::mat4(1.0f));
	m_globalJointPositions.resize(boneCount, glm::vec3(0.0f));

	return true;
}

void Animator::Update()
{
    // 모델이 없으면 애니메이션 재생 불가
    if (!m_currentModel) return;

    // 1. 시간 업데이트 (애니메이션 리소스가 있을 때만 진행)
    if (m_currentAnimation)
    {
        float dt = TIME.GetDeltaTime(); // 싱글톤 접근
        m_currentTime += m_currentAnimation->GetTicksPerSecond() * dt;

        float duration = m_currentAnimation->GetDuration();
        // 0으로 나누기 방지 및 루프 처리
        if (duration > 0.0f)
        {
            m_currentTime = fmod(m_currentTime, duration);
        }
    }

    // 2. [핵심] 계층 구조 순회 (선형 반복)
    // ModelConverter에서 이미 "부모가 항상 자식보다 앞 인덱스에 오도록" 정렬해둠.
    const auto& nodes = m_currentModel->GetNodes();

    // 현재 프레임의 글로벌 변환 행렬들을 담을 임시 버퍼
    // (매 프레임 할당이 부담된다면 멤버 변수 m_globalTransforms로 빼도 됨)
    std::vector<glm::mat4> globalTransforms(nodes.size());

    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const auto& node = nodes[i];

        // A. 기본 로컬 변환 (T-Pose 상태)
        glm::mat4 localTransform = node.localTransform;

        // B. 애니메이션 적용 (채널이 존재하면 덮어쓰기)
        if (m_currentAnimation)
        {
            // 이름으로 채널 검색 (FindBone -> FindChannel 권장)
            const AnimChannel* channel = m_currentAnimation->FindBone(node.name);

            if (channel)
            {
                // [Stateless] 현재 시간을 인자로 넘겨서 행렬을 받아옴
                localTransform = channel->GetLocalTransform(m_currentTime);
            }
        }

        // C. 부모 행렬과 결합 (Global Transform 계산)
        int32 parentIndex = node.parentIndex;

        if (parentIndex == -1) // 루트 노드
        {
            globalTransforms[i] = localTransform;
        }
        else
        {
            // 부모는 무조건 나보다 인덱스가 작으므로, 이미 globalTransforms에 계산되어 있음
            globalTransforms[i] = globalTransforms[parentIndex] * localTransform;
        }

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
                m_finalBoneMatrices[boneID] = globalTransforms[i] * offsetMat;

                // 바운딩 박스 계산 등을 위해 관절 위치 저장
                m_globalJointPositions[boneID] = glm::vec3(globalTransforms[i][3]);
            }
        }
    }

    // 3. 바운딩 박스 업데이트
    CalculateCurrentPoseLocalBounds();
}

//void Animator::Update()
//{
//    if (!m_currentModel) return;
//
//    // 1. 시간 업데이트 (애니메이션이 있을 때만)
//    if (m_currentAnimation)
//    {
//        float dt = TIME.GetDeltaTime();
//        m_currentTime += m_currentAnimation->GetTicksPerSecond() * dt;
//        m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());
//    }
//
//    // 2. [핵심] 선형 반복문으로 계층 구조 계산
//    // ModelConverter에서 부모가 항상 자식보다 앞 인덱스에 오도록 정렬했으므로 
//    // 0번부터 순서대로 계산하면 됨.
//    const auto& nodes = m_currentModel->GetNodes(); // vector<RawNode>
//
//    // 현재 프레임의 글로벌 변환 행렬들을 담을 임시 버퍼
//    // (최적화를 위해 멤버 변수로 빼서 재사용 가능)
//    std::vector<glm::mat4> globalTransforms(nodes.size());
//
//    for (size_t i = 0; i < nodes.size(); ++i)
//    {
//        const auto& node = nodes[i];
//
//        // A. 기본 로컬 변환 (T-Pose)
//        glm::mat4 localTransform = node.localTransform;
//
//        // B. 애니메이션 적용 (있으면)
//        if (m_currentAnimation)
//        {
//            // Animation에서 해당 노드 이름의 채널 찾기
//            const AnimChannel* channel = m_currentAnimation->FindBone(node.name);
//            if (channel)
//            {
//                // [변경] Stateless: 시간만 주면 행렬 리턴 (const 호환)
//                localTransform = channel->GetLocalTransform();
//            }
//        }
//
//        // C. 부모 행렬과 결합 (Global Transform 계산)
//        // parentIndex가 -1이면 루트
//        if (node.parentIndex == -1)
//        {
//            globalTransforms[i] = localTransform;
//        }
//        else
//        {
//            // 부모는 무조건 나보다 인덱스가 작으므로 이미 계산됨 (정렬 보장)
//            globalTransforms[i] = globalTransforms[node.parentIndex] * localTransform;
//        }
//
//        // D. 스키닝 행렬(FinalBoneMatrix) 계산
//        // 이 노드가 실제 뼈(Bone)인지 확인
//        const auto& boneInfoMap = m_currentModel->GetBoneInfoMap();
//        auto it = boneInfoMap.find(node.name);
//
//        if (it != boneInfoMap.end())
//        {
//            uint32 boneID = it->second.id;
//            const glm::mat4& offsetMat = it->second.offset;
//
//            // 범위 체크 후 적용
//            if (boneID < m_finalBoneMatrices.size())
//            {
//                // Final = Global * Offset
//                m_finalBoneMatrices[boneID] = globalTransforms[i] * offsetMat;
//
//                // 디버그용 위치 저장
//                m_globalJointPositions[boneID] = glm::vec3(globalTransforms[i][3]);
//            }
//        }
//    }
//
//    // 3. 바운딩 박스 업데이트
//    CalculateCurrentPoseLocalBounds();
//
//	/*if (m_currentAnimation)
//	{
//		m_currentTime += m_currentAnimation->GetTicksPerSecond() * TIME.GetDeltaTime();
//		m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());
//		CalculateBoneTransform(&m_currentAnimation->GetRootNode(), glm::mat4(1.0f));
//		CalculateCurrentPoseLocalBounds();
//	}*/
//}

void Animator::PlayAnimation(AnimationPtr animation)
{
	m_currentAnimation = std::move(animation);
	m_currentTime = 0.0f;
}

void Animator::CalculateCurrentPoseLocalBounds()
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