#include "FrameworkPch.h"
#include "AnimationManager.h"
#include "Managers/EntityManager.h"
#include "Instancing/Animations/AnimationBuffer.h"
#include "Components/Animation/Animator.h"
#include "Components/MeshRenderers/SkinnedMeshRenderer.h"

namespace MGF3D
{
	AnimationManager::AnimationManager() = default;
	AnimationManager::~AnimationManager() = default;

	bool AnimationManager::Init()
	{
		m_boneBuffer = AnimationBufferUPtr(new AnimationBuffer());
		if (!m_boneBuffer->Init(10000))
		{
			MGF_LOG_ERROR("AnimationManager: Failed to initialize AnimationBuffer.");
			return false;
		}

		return true;
	}

	void AnimationManager::Update(float dt)
	{
		if (m_boneBuffer == nullptr) return;

		// 1. 프레임 시작 시 글로벌 버퍼의 오프셋 초기화
		m_boneBuffer->Clear();

		// 2. ECS 레지스트리에서 Animator 컨테이너 가져오기
		auto animatorRegistry = MGF_ENTITY.GetComponentRegistry<Animator>();
		if (animatorRegistry == nullptr) return;

		// 3. 모든 Animator 선형 순회
		for (auto& animator : animatorRegistry->GetComponents())
		{
			animator->Update(dt);

			// Animator에서 미리 계산된 이번 프레임의 최종 뼈대 행렬 배열을 가져옵니다.
			const Vector<mat4>& finalBones = animator->GetFinalBoneMatrices();
			if (finalBones.empty()) continue;

			// 버퍼에 뼈대 데이터를 밀어넣고, 이 데이터가 위치한 시작 인덱스(Offset)를 반환받습니다.
			uint32 offset = m_boneBuffer->PushBones(finalBones);

			// 동일한 Owner ID를 가진 SkinnedMeshRenderer를 찾습니다.
			animator->SetBoneBufferOffset(offset);
		}

		// 4. 수집이 끝난 뒤 단 한 번의 호출로 GPU 버퍼에 데이터를 전송합니다.
		m_boneBuffer->UploadToGPU();
	}

	void AnimationManager::Shutdown()
	{
		if (m_boneBuffer) m_boneBuffer.reset();
	}
}