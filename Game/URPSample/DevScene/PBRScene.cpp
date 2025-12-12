#include "EnginePch.h"
#include "PBRScene.h"

#include "Core/GameObject.h"
#include "Core/Renderer.h"
#include "Graphics/Program.h"
#include "Graphics/CubeFramebuffer.h"
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
#include "Graphics/SkyLight.h"
#include "Audios/AudioClip.h"

#include "Misc/IBLUtils.h"

#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/SpotLight.h"
#include "Components/DirectionalLight.h"
#include "Components/PointLight.h"
#include "Components/Animator.h"
#include "Components/AudioSource.h"
#include "Components/AudioListener.h"

#include "URPSample/RenderPasses/HDRRenderPass.h"
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
	// 큐브 메쉬
	{
		auto cubeMesh = GeometryGenerator::CreateBox();
		if (!cubeMesh) return false;
		RESOURCE.AddResource<Mesh>("Cube", std::move(cubeMesh));
	}

	// 구 메쉬
	{
		auto sphereMesh = GeometryGenerator::CreateSphere();
		if (!sphereMesh) return false;
		RESOURCE.AddResource<Mesh>("Sphere", std::move(sphereMesh));
	}

	// 단색 머티리얼
	{
		auto solidColorMat = Material::Create();
		if (!solidColorMat) return false;
		solidColorMat->diffuse = Texture::CreateFromImage(Image::CreateSingleColorImage
		(4, 4, glm::vec4(0.75f, 0.1f, 0.2f, 1.0f)).get());
		solidColorMat->metallic = Texture::CreateFromImage(Image::CreateSingleColorImage
		(4, 4, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f)).get());
		solidColorMat->roughness = Texture::CreateFromImage(Image::CreateSingleColorImage
		(4, 4, glm::vec4(0.25f, 0.35f, 0.25f, 1.0f)).get());
		RESOURCE.AddResource<Material>("solidColor", std::move(solidColorMat));
	}

	// 쇠공 머티리얼
	{
		// TODO : 이후에는 ktx로 한 번 구울 필요가 있음.
		auto rustedIronMat = Material::Create();
		if (!rustedIronMat) return false;
		rustedIronMat->diffuse = Texture::CreateFromImage
		(Image::Load("./Resources/Images/rustediron/rustediron2_basecolor.png").get());
		rustedIronMat->roughness = Texture::CreateFromImage
		(Image::Load("./Resources/Images/rustediron/rustediron2_roughness.png").get());
		rustedIronMat->metallic = Texture::CreateFromImage
		(Image::Load("./Resources/Images/rustediron/rustediron2_metallic.png").get());
		rustedIronMat->normal = Texture::CreateFromImage
		(Image::Load("./Resources/Images/rustediron/rustediron2_normal.png").get());
		RESOURCE.AddResource<Material>("Rusted_Iron", std::move(rustedIronMat));
	}

	// HDR 큐브맵 머티리얼
	{
		auto hdrCubeMat = Material::Create();
		if (!hdrCubeMat) return false;
		hdrCubeMat->diffuse = Texture::CreateFromHDR
		(Image::LoadHDR("./Resources/Images/IBL/mirrored_hall_4k.hdr").get());
		RESOURCE.AddResource<Material>("hdrCubeMat", std::move(hdrCubeMat));
	}

	// HDR Skybox 생성 (IBLUtils 사용)
	{
		auto hdrImage = Image::LoadHDR("./Resources/Images/IBL/mirrored_hall_4k.hdr");
		auto hdrTex = Texture::CreateFromHDR(hdrImage.get());

		CubeTexturePtr bakedCubemap = IBLUtils::CreateCubemapFromHDR(hdrTex.get());
		CubeTexturePtr bakedDiffuse = IBLUtils::CreateIrradianceMap(bakedCubemap.get());
		CubeTexturePtr prefiltered  = IBLUtils::CreatePrefilteredMap(bakedCubemap.get());
		TexturePtr	   brdf			= IBLUtils::CreateBRDFLUT();

		auto sky = SkyLight::CreateUniversalSky
		(
			std::move(bakedCubemap), std::move(bakedDiffuse), 
			std::move(prefiltered),  std::move(brdf)
		);
		SetSkyLight(std::move(sky));
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
		AddCustomRenderPass("simpleHDR", HDRRenderPass::Create(std::move(prog)));
	}

	return true;
}

bool PBRScene::CreateSceneContext()
{
	// 0. 추가한 렌더패스 가져오기
	HDRRenderPass* hdrPass = (HDRRenderPass*)GetCustomRenderPass("simpleHDR");

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

	// 2. 조명 추가
	{
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
	}

	// 3. 구 49개 (PBR Chart)
	{
		// 리소스 가져오기
		auto sphereMesh = RESOURCE.GetResource<Mesh>("Sphere");

		// [공유 리소스] 모든 구가 공유할 기본 알베도 (빨간색)
		TexturePtr sharedAlbedo = Texture::CreateFromImage(
			Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get()
		);

		// 7x7 그리드 설정
		const int rows = 7;
		const int cols = 7;
		const float spacing = 1.4f;

		for (int row = 0; row < rows; ++row)
		{
			// Row를 기준으로 메탈릭 계산 (0: 아래쪽 ~ 6: 위쪽)
			// 위쪽(Top)일수록 메탈릭이 1.0이 되어야 하므로 row 비례
			float metallicFactor = (float)row / (float)(rows - 1);

			float y = ((float)row - (float)(rows - 1) * 0.5f) * spacing;

			for (int col = 0; col < cols; ++col)
			{
				// Col을 기준으로 거칠기 계산 (0: 왼쪽 ~ 6: 오른쪽)
				// 왼쪽(Left)일수록 거칠기가 0.0이어야 하므로 col 비례
				float roughnessFactor = (float)col / (float)(cols - 1);
				// 거칠기가 완전 0이면 렌더링 시 아티팩트가 생길 수 있으니 최소값 보정 (0.05f ~ 1.0f)
				roughnessFactor = glm::clamp(roughnessFactor, 0.05f, 1.0f);

				// X 좌표 계산
				float x = ((float)col - (float)(cols - 1) * 0.5f) * spacing;

				// 1. GameObject 생성
				auto sphereObj = GameObject::Create();
				sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

				// 2. 머티리얼 인스턴스 생성 (각 구마다 속성이 다르므로 개별 생성)
				MaterialPtr pbrMat = Material::Create();

				// 알베도와 AO는 공유
				pbrMat->diffuse = sharedAlbedo;

				// 메탈릭 텍스처 생성 (단색 4x4)
				pbrMat->metallic = Texture::CreateFromImage(
					Image::CreateSingleColorImage(4, 4, glm::vec4(metallicFactor, metallicFactor, metallicFactor, 1.0f)).get()
				);

				// 러프니스 텍스처 생성 (단색 4x4)
				pbrMat->roughness = Texture::CreateFromImage(
					Image::CreateSingleColorImage(4, 4, glm::vec4(roughnessFactor, roughnessFactor, roughnessFactor, 1.0f)).get()
				);

				// 3. MeshRenderer 생성 및 연결
				auto mr = MeshRenderer::Create(sphereMesh, pbrMat);
				hdrPass->AddRenderer(mr.get()); // 렌더 패스에 등록

				sphereObj->AddComponent(std::move(mr));
				AddGameObject(std::move(sphereObj));
			}
		}
	}

	//// 3. 구 49개 (PBR Chart)
	//{
	//	// 리소스 가져오기
	//	auto sphereMesh = RESOURCE.GetResource<Mesh>("Sphere");

	//	// [최적화] 모든 구가 공유할 기본 텍스처는 미리 만들어둡니다.
	//	// 빨간색 알베도 (R=1.0, G=0.0, B=0.0)
	//	TexturePtr sharedAlbedo = Texture::CreateFromImage(
	//		Image::CreateSingleColorImage(1, 1, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)).get()
	//	);

	//	// AO는 1.0 (그림자 없음)
	//	auto sharedAO = Texture::CreateBlack();

	//	// 7x7 그리드 설정
	//	const int rows = 7;
	//	const int cols = 7;
	//	const float spacing = 1.4f;

	//	for (int row = 0; row < rows; ++row)
	//	{
	//		float y = ((float)row - (float)(rows - 1) * 0.5f) * spacing;

	//		for (int col = 0; col < cols; ++col)
	//		{
	//			// X 좌표 계산
	//			float x = ((float)col - (float)(cols - 1) * 0.5f) * spacing;

	//			// 1. GameObject 생성
	//			auto sphereObj = GameObject::Create();
	//			sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

	//			// 2. MeshRendere 생성
	//			auto mr = MeshRenderer::Create(sphereMesh, RESOURCE.GetResource<Material>("solidColor"));
	//			hdrPass->AddRenderer(mr.get()); // 렌더 패스에 등록

	//			sphereObj->AddComponent(std::move(mr));
	//			AddGameObject(std::move(sphereObj));
	//		}
	//	}
	//}

	return true;
}