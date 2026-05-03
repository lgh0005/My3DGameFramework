#include "GamePch.h"
#include "TestScene.h"
#include "Managers/TypeManager.h"
#include "Managers/AssetManager.h"
#include "Managers/ResourceManager.h"

#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/MeshRenderers/MeshRenderer.h"
#include "Components/Lights/PointLight.h"

#include "Meshes/StaticMesh.h"
#include "Resources/Material.h"
#include "GraphicsUtils/GeometryUtils.h"

#include "Scripts/CameraController.h"

#include "Assets/Image.h"
#include "Assets/Model.h"

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

	bool TestScene::OnLoadSceneSources()
	{
		auto sharedMaterial = MGF_RESOURCE.GetOrCreate<Material>("SharedCubeMaterial");
		sharedMaterial->SetTexture(ETextureSlot::Albedo, MGF_ASSET.LoadAssetAsync<Image>("@GameAsset/Images/baked/brickwall.ktx"));
		sharedMaterial->SetTexture(ETextureSlot::Normal, MGF_ASSET.LoadAssetAsync<Image>("@GameAsset/Images/baked/brickwall_normal.ktx"));

		m_backpackModel = MGF_ASSET.LoadAssetAsync<Model>("@GameAsset/Models/backpack/backpack.mymodel");

		return true;
	}

	bool TestScene::OnPlaceActors()
	{
		// 1. 객체 생성 [카메라]
		ObjectIDHash playerID = Entities::Create("MainPlayer");
		auto* transform = Entities::AddComponent<Transform>(playerID);
		auto* camera = Entities::AddComponent<Camera>(playerID);
		auto* controller = Scripts::AddScript<CameraController>(playerID);
		transform->SetLocalPosition(vec3(0.0f, 0.0f, 5.0f));
		camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		// [핵심] 자원은 메모리에 딱 한 번만 생성하여 공유합니다.
		StaticMeshPtr sharedMesh = GeometryUtils::CreateSphere();
		MaterialPtr sharedMaterial = MGF_RESOURCE.Get<Material>("SharedCubeMaterial");

		// 2. 객체 생성 [첫 번째 큐브]
		ObjectIDHash cubeID1 = Entities::Create("Cube1");
		auto* cubeTransform1 = Entities::AddComponent<Transform>(cubeID1);
		Entities::AddComponent<MeshRenderer>(cubeID1, sharedMesh, sharedMaterial);
		cubeTransform1->SetLocalPosition(vec3(-1.5f, 0.0f, 0.0f));

		// 3. 객체 생성 [두 번째 큐브]
		ObjectIDHash cubeID2 = Entities::Create("Cube2");
		auto* cubeTransform2 = Entities::AddComponent<Transform>(cubeID2);
		Entities::AddComponent<MeshRenderer>(cubeID2, sharedMesh, sharedMaterial);
		cubeTransform2->SetLocalPosition(vec3(1.5f, 0.0f, 0.0f));

		// 4. 객체 생성 [포인트 라이트]
		ObjectIDHash lightID = Entities::Create("PointLight1");
		auto* lightTransform = Entities::AddComponent<Transform>(lightID);
		auto* pointLight = Entities::AddComponent<PointLight>(lightID);
		lightTransform->SetLocalPosition(vec3(0.0f, 1.0f, 2.0f));
		pointLight->SetColor(vec3(1.0f, 0.0f, 0.0f));
		pointLight->SetIntensity(2.0f);
		pointLight->SetRange(10.0f);

		// 5. 가방
		if (m_backpackModel != nullptr)
		{
			// 핵심: 상태가 Loading인 동안에는 메인 스레드가 다음 줄로 넘어가지 못하게 루프에 가둡니다.
			while (m_backpackModel->GetState() == EAssetState::Loading)
			{
				// CPU 점유율이 폭주하지 않도록 다른 스레드(워커 스레드)에게 연산을 양보합니다.
				std::this_thread::yield();
			}

			// 루프를 빠져나왔다면 워커 스레드의 작업이 끝난 상태입니다. (Loaded 또는 Failed)
			if (m_backpackModel->GetState() == EAssetState::Loaded)
			{
				ObjectIDHash backpackID = m_backpackModel->Instantiate("Backpack");

				if (backpackID.IsValid())
				{
					auto bagTransform = Entities::GetComponent<Transform>(backpackID);
					if (bagTransform != nullptr)
					{
						// 카메라가 (0,0,5)에 있으므로, 카메라 바로 앞인 (0,0,0)에 배치!
						bagTransform->SetLocalPosition(vec3(0.0f, 0.0f, -10.0f));

						// 크기를 1배, 혹은 10배로 무식하게 키워봅니다!
						bagTransform->SetLocalScale(vec3(0.0125, 0.0125, 0.0125));

						MGF_LOG_INFO(u8"✨ 가방 스폰 로직 통과! 현재 위치: 0,0,0 / 스케일: 10.0");
					}
				}
			}
			else if (m_backpackModel->GetState() == EAssetState::Failed)
			{
				MGF_LOG_ERROR(u8"❌ 에셋 매니저에서 가방 로딩을 실패(Failed) 처리했습니다!");
			}
		}

		auto comp = MGF_ENTITY.GetComponentRegistry<MeshRenderer>()->GetComponents().GetSize();

		return true;
	}
}