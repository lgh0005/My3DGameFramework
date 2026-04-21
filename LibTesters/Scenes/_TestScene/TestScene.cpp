#include "GamePch.h"
#include "TestScene.h"
#include "Managers/TypeManager.h"

#include "Components/Transform.h"
#include "Components/Camera.h"

namespace MGF3D
{
	TestScene::TestScene() = default;
	TestScene::~TestScene() = default;

	/*================================//
	//    MGFInputDevice Custom Type  //
	//================================*/
	int16 TestScene::s_typeIndex = -1;
	const MGFType* TestScene::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Scene");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool TestScene::OnLoadSceneResources()
	{
		MGF_LOG_FATAL("TestScene : OnLoadSceneResources called.");
		return true;
	}

	bool TestScene::OnPlaceActors()
	{
		MGF_LOG_FATAL("TestScene : OnPlaceActors called.");

		MGF_LOG_WARN("--- [Verification Phase] Starting Entity & Component Setup ---");

		// 1. 객체 생성
		ObjectIDHash playerID = Entities::Create("MainPlayer");

		if (playerID.IsValid())
		{
			// 2. 컴포넌트 추가
			auto* trn = Entities::AddComponent<Transform>(playerID);
			auto* cam = Entities::AddComponent<Camera>(playerID);

			MGF_LOG_WARN("Components added to MainPlayer. Now verifying via EntityManager...");

			// 3. GetComponent를 통한 역추적 및 확인
			// 이 과정에서 우리가 구현한 O(1) m_registryLookup이 작동합니다.
			Transform* verifiedTransform = Entities::GetComponent<Transform>(playerID);
			Camera* verifiedCamera = Entities::GetComponent<Camera>(playerID);

			// Transform 검증
			if (verifiedTransform != nullptr)
			{
				verifiedTransform->SetLocalPosition(vec3(0.0f, 0.0f, -5.0f));
				MGF_LOG_WARN("[Verify Success] Transform retrieved from EntityManager.");
				MGF_LOG_WARN(" -> Memory Address: {0}", (void*)verifiedTransform);

				// 데이터 수량화 테스트 (초기값 확인)
				vec3 pos = verifiedTransform->GetLocalPosition();
				MGF_LOG_WARN(" -> Initial Position: ({0}, {1}, {2})", pos.x, pos.y, pos.z);
			}
			else
			{
				MGF_LOG_WARN("[Verify Failed] Transform could not be found in EntityManager!");
			}

			// Camera 검증
			if (verifiedCamera != nullptr)
			{
				MGF_LOG_WARN("[Verify Success] Camera retrieved from EntityManager.");
				MGF_LOG_WARN(" -> Memory Address: {0}", (void*)verifiedCamera);
				MGF_LOG_WARN(" -> Current FOV: {0}", verifiedCamera->GetFOV());
			}
			else
			{
				MGF_LOG_WARN("[Verify Failed] Camera could not be found in EntityManager!");
			}
		}

		MGF_LOG_INFO("--- [Verification Phase] Completed ---");

		return true;
	}
}