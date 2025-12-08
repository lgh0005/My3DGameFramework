#include "EnginePch.h"
#include "PBRScene.h"

#include "Core/GameObject.h"
#include "Core/Renderer.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/SkinnedMesh.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Image.h"
#include "Graphics/Animation.h"
#include "Graphics/Model.h"
#include "Graphics/Buffer.h"
#include "Graphics/InstancedMesh.h"
#include "Graphics/Geometry.h"
#include "Audios/AudioClip.h"

#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/SpotLight.h"
#include "Components/DirectionalLight.h"
#include "Components/PointLight.h"
#include "Components/Animator.h"
#include "Components/AudioSource.h"
#include "Components/AudioListener.h"

#include "URPSample/RenderPasses/TestRenderPass.h"
#include "SRPSample/Scripts/CameraController.h"

PBRScene::~PBRScene() = default;

PBRSceneUPtr PBRScene::Create()
{
	auto pbrScene = PBRSceneUPtr(new PBRScene());
	if (!pbrScene->Init()) return nullptr;
	return std::move(pbrScene);
}

bool PBRScene::LoadNessesaryResources()
{
	// 구 메쉬
	{
		auto sphereMesh = GeometryGenerator::CreateSphere();
		if (!sphereMesh) return false;
		RESOURCE.AddResource<Mesh>("Sphere", std::move(sphereMesh));
	}

	// 간단한 머티리얼
	{
		auto simpleMat = Material::Create();
		if (!simpleMat) return false;
		simpleMat->roughness = Texture::CreateFromFloat(0.15f);
		simpleMat->metallic = Texture::CreateFromFloat(1.0f);
		simpleMat->shininess = 16.0f;
		simpleMat->emissionStrength = 0.0f;
		simpleMat->heightScale = 0.0f;
		RESOURCE.AddResource<Material>("LightMat", std::move(simpleMat));
	}

	return true;
}

bool PBRScene::CreateNessesaryRenderPasses()
{
	// 간단한 머티리얼 포워드 렌더 패스
	{
		auto prog = Program::Create
		(
			"./Resources/Shaders/Universal/test_pbr.vert",
			"./Resources/Shaders/Universal/test_pbr.frag"
		); if (!prog) return false;
		AddCustomRenderPass("SimpleDimple", TestRenderPass::Create(std::move(prog)));
	}

	return true;
}

bool PBRScene::CreateSceneContext()
{
	// 0. 추가한 렌더패스 가져오기
	TestRenderPass* lightPass = (TestRenderPass*)GetCustomRenderPass("SimpleDimple");

	// 1. 카메라 오브젝트 추가
	{
		// GameObject 생성 및 씬에 등록
		auto cameraObj = GameObject::Create(); if (!cameraObj) return false;
		auto camera = Camera::Create();		   if (!camera)	   return false;
		auto* cameraPtr = camera.get();

		// 카메라 컴포넌트 생성 및 추가
		cameraObj->GetTransform().SetPosition(glm::vec3(0.0f, 2.5f, 8.0f));
		camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 100.0f);
		cameraObj->AddComponent(std::move(camera));

		// 카메라 컨트롤러 생성 및 추가
		auto cameraCtrl = CameraController::Create();
		cameraObj->AddComponent(std::move(cameraCtrl));

		SetMainCamera(cameraPtr); // 메인 카메라로 설정
		AddGameObject(std::move(cameraObj));
	}

	// 2. 조명 추가 1
	{
		auto lightGo = GameObject::Create();
		lightGo->SetName("PointLight1");
		auto lightComp = PointLight::Create();
		lightGo->GetTransform().SetPosition(glm::vec3(5.0f, 5.0f, 4.0f));
		lightGo->GetTransform().SetScale(glm::vec3(0.2f));
		lightGo->AddComponent(std::move(lightComp));
		AddGameObject(std::move(lightGo));
	}

	// 2. 조명 추가 2
	{
		auto lightGo = GameObject::Create();
		lightGo->SetName("PointLight2");
		auto lightComp = PointLight::Create();
		lightGo->GetTransform().SetPosition(glm::vec3(-4.0f, 5.0f, 5.0f));
		lightGo->GetTransform().SetScale(glm::vec3(0.2f));
		lightGo->AddComponent(std::move(lightComp));
		AddGameObject(std::move(lightGo));
	}

	// 2. 조명 추가 3
	{
		auto lightGo = GameObject::Create();
		lightGo->SetName("PointLight3");
		auto lightComp = PointLight::Create();
		lightGo->GetTransform().SetPosition(glm::vec3(-4.0f, -6.0f, 6.0f));
		lightGo->GetTransform().SetScale(glm::vec3(0.2f));
		lightGo->AddComponent(std::move(lightComp));
		AddGameObject(std::move(lightGo));
	}

	// 2. 조명 추가 4
	{
		auto lightGo = GameObject::Create();
		lightGo->SetName("PointLight4");
		auto lightComp = PointLight::Create();
		lightGo->GetTransform().SetPosition(glm::vec3(5.0f, -6.0f, 7.0f));
		lightGo->GetTransform().SetScale(glm::vec3(0.2f));
		lightGo->AddComponent(std::move(lightComp));
		AddGameObject(std::move(lightGo));
	}

	//// 3. 구 49개
	//{
	//	// 리소스 가져오기 (LoadNessesaryResources에서 로드한 것들)
	//	auto sphereMesh = RESOURCE.GetResource<Mesh>("Sphere");
	//	auto lightMat = RESOURCE.GetResource<Material>("LightMat");

	//	// 7x7 그리드 생성 루프
	//	const int sphereCount = 7;
	//	const float offset = 1.4f;

	//	for (int row = 0; row < sphereCount; row++)
	//	{
	//		// 중앙 정렬을 위한 Y 좌표 계산
	//		float y = ((float)row - (float)(sphereCount - 1) * 0.5f) * offset;

	//		for (int col = 0; col < sphereCount; col++)
	//		{
	//			// 중앙 정렬을 위한 X 좌표 계산
	//			float x = ((float)col - (float)(sphereCount - 1) * 0.5f) * offset;

	//			// GameObject 생성
	//			auto sphereObj = GameObject::Create();

	//			// Transform 설정
	//			sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

	//			// MeshRenderer 컴포넌트 생성
	//			auto mr = MeshRenderer::Create(sphereMesh, lightMat);
	//			mr->SetMesh(sphereMesh);
	//			mr->SetMaterial(lightMat);

	//			lightPass->AddRenderer(mr.get());

	//			// 컴포넌트 및 오브젝트 등록
	//			sphereObj->AddComponent(std::move(mr));
	//			AddGameObject(std::move(sphereObj));
	//		}
	//	}
	//}

	// 3. 구 49개 (PBR Chart)
	{
		// 리소스 가져오기
		auto sphereMesh = RESOURCE.GetResource<Mesh>("Sphere");

		// [최적화] 모든 구가 공유할 기본 텍스처는 미리 만들어둡니다.
		// 빨간색 알베도 (R=1.0, G=0.0, B=0.0)
		TexturePtr sharedAlbedo = Texture::CreateFromImage(
			Image::CreateSingleColorImage(1, 1, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)).get()
		);

		// AO는 1.0 (그림자 없음)
		auto sharedAO = Texture::CreateFromFloat(1.0f);

		// 7x7 그리드 설정
		const int rows = 7;
		const int cols = 7;
		const float spacing = 1.4f;

		for (int row = 0; row < rows; ++row)
		{
			// [Metallic 계산]
			// row가 0(맨 아래) -> 6(맨 위)으로 갈수록 0.0 -> 1.0 증가
			// "좌상단이 메탈릭이 높고" -> 위쪽(Row가 큰 쪽)이 금속
			float metallicValue = (float)row / (float)(rows - 1);

			// Y 좌표 계산 (중앙 정렬)
			float y = ((float)row - (float)(rows - 1) * 0.5f) * spacing;

			for (int col = 0; col < cols; ++col)
			{
				// [Roughness 계산]
				// col이 0(맨 왼쪽) -> 6(맨 오른쪽)으로 갈수록 0.0 -> 1.0 증가
				// "좌상단이 러프니스가 낮고" -> 왼쪽(Col이 작은 쪽)이 매끈함
				// *주의: Roughness 0.0은 수학적으로 문제가 될 수 있어 0.05부터 시작
				float roughnessValue = glm::clamp((float)col / (float)(cols - 1), 0.05f, 1.0f);

				// X 좌표 계산
				float x = ((float)col - (float)(cols - 1) * 0.5f) * spacing;

				// 1. GameObject 생성
				auto sphereObj = GameObject::Create();
				sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

				// 2. [핵심] 인스턴스별 고유 머티리얼 생성
				auto instanceMat = Material::Create();

				// 공유 텍스처 재사용 (메모리 절약)
				instanceMat->diffuse = sharedAlbedo; // Albedo
				instanceMat->ao = sharedAO;          // AO

				// 고유 텍스처 생성 (Texture::CreateFromFloat 활용)
				instanceMat->metallic = Texture::CreateFromFloat(metallicValue);
				instanceMat->roughness = Texture::CreateFromFloat(roughnessValue);

				// 기타 속성 초기화
				instanceMat->normal = nullptr; // 없으면 Blue 자동 바인딩
				instanceMat->emission = nullptr;
				instanceMat->emissionStrength = 0.0f;

				// 3. 렌더러 생성 및 등록
				// MeshRenderer가 Material의 소유권(shared_ptr 등)을 잘 관리한다고 가정
				auto mr = MeshRenderer::Create(sphereMesh, std::move(instanceMat));

				lightPass->AddRenderer(mr.get()); // 렌더 패스에 등록

				sphereObj->AddComponent(std::move(mr));
				AddGameObject(std::move(sphereObj));
			}
		}
	}

	return true;
}