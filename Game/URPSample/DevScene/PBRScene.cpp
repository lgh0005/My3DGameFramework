#include "EnginePch.h"
#include "PBRScene.h"

#include "Core/GameObject.h"
#include "Core/Renderer.h"
#include "Graphics/Program.h"
#include "Graphics/CubeFramebuffer.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/SkinnedMesh.h"
#include "Resources/Material.h"
#include "Resources/Texture.h"
#include "Resources/CubeTexture.h"
#include "Resources/Image.h"
#include "Resources/Animation.h"
#include "Resources/Model.h"
#include "Resources/InstancedMesh.h"
#include "Resources/AudioClip.h"
#include "Graphics/Buffer.h"
#include "Graphics/Geometry.h"
#include "Graphics/SkyLight.h"

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
		RESOURCE.AddResource<StaticMesh>(std::move(cubeMesh), "Cube");
	}

	// 구 메쉬
	{
		auto sphereMesh = GeometryGenerator::CreateSphere();
		if (!sphereMesh) return false;
		RESOURCE.AddResource<StaticMesh>(std::move(sphereMesh), "Sphere");
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
		RESOURCE.AddResource<Material>(std::move(solidColorMat), "solidColor");
	}

	// 쇠공 머티리얼 #1
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
		RESOURCE.AddResource<Material>(std::move(rustedIronMat), "Rusted_Iron");
	}

	// 쇠공 머티리얼 #2
	{
		// TODO : 이후에는 ktx로 한 번 구울 필요가 있음.
		auto rustedIronMat = Material::Create();
		if (!rustedIronMat) return false;
		rustedIronMat->diffuse = Texture::CreateFromImage
		(Image::Load("./Resources/Images/rustediron/rustediron2_basecolor.png").get());
		rustedIronMat->normal = Texture::CreateFromImage
		(Image::Load("./Resources/Images/rustediron/rustediron2_normal.png").get());
		rustedIronMat->orm = Texture::CreateFromImage
		(Image::Load("./Resources/Images/rustediron/rustediron2_ORM.png").get());
		RESOURCE.AddResource<Material>(std::move(rustedIronMat), "Rusted_Iron_orm");
	}

	// HDR 큐브맵 머티리얼
	{
		auto hdrCubeMat = Material::Create();
		if (!hdrCubeMat) return false;
		hdrCubeMat->diffuse = Texture::CreateFromHDR
		(Image::LoadHDR("./Resources/Images/IBL/mirrored_hall_4k.hdr").get());
		RESOURCE.AddResource<Material>(std::move(hdrCubeMat), "hdrCubeMat");
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
			"./Resources/Shaders/Universal/test_pbr_testing.vert",
			"./Resources/Shaders/Universal/test_pbr_testing.frag"
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
	
	// 3. 구 49개 (ORM 텍스쳐 테스트)
	{
		auto sphereMesh = RESOURCE.GetResource<StaticMesh>("Sphere");

		// [핵심] 미리 로드해둔 'Rusted_Iron_orm' 머티리얼을 가져옵니다.
		// 이 머티리얼 안에는 이미 Diffuse, Normal, ORM 텍스처가 들어있습니다.
		auto baseMat = RESOURCE.GetResource<Material>("Rusted_Iron_orm");

		const int rows = 7;
		const int cols = 7;
		const float spacing = 1.4f;

		for (int row = 0; row < rows; ++row)
		{
			// Metallic Factor (0.0 ~ 1.0)
			float metallicFactor = (float)row / (float)(rows - 1);
			float y = ((float)row - (float)(rows - 1) * 0.5f) * spacing;

			for (int col = 0; col < cols; ++col)
			{
				// Roughness Factor (0.05 ~ 1.0)
				float roughnessFactor = (float)col / (float)(cols - 1);
				roughnessFactor = glm::clamp(roughnessFactor, 0.05f, 1.0f);

				float x = ((float)col - (float)(cols - 1) * 0.5f) * spacing;

				auto sphereObj = GameObject::Create();
				sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

				// [중요] 각 구체마다 개별적인 파라미터를 가져야 하므로 새 머티리얼 생성
				MaterialPtr pbrMat = Material::Create();

				// 1. 텍스처 공유 (Texture Pointer Copy)
				// 이미지를 새로 로드하는게 아니라, baseMat이 들고 있는 텍스처 포인터만 복사합니다.
				// 따라서 메모리 낭비 없이 텍스처를 재사용합니다.
				if (baseMat)
				{
					pbrMat->diffuse = baseMat->diffuse;
					pbrMat->normal = baseMat->normal;
					pbrMat->orm = baseMat->orm; // ORM 텍스처 연결
				}

				// 2. Factor 값 설정 (Grid Logic)
				// 셰이더 연산: FinalValue = TextureValue * FactorValue
				pbrMat->albedoFactor = glm::vec4(1.0f); // 원본 텍스처 색상 그대로 (Tint 없음)
				pbrMat->metallicFactor = metallicFactor;  // 행에 따라 금속성 조절
				pbrMat->roughnessFactor = roughnessFactor; // 열에 따라 거칠기 조절

				// 렌더러 등록
				auto mr = MeshRenderer::Create(sphereMesh, pbrMat);
				hdrPass->AddRenderer(mr.get());
				sphereObj->AddComponent(std::move(mr));
				AddGameObject(std::move(sphereObj));
			}
		}
	}

	// 4. 구 49개 (PBR Chart)
	{
		//auto sphereMesh = RESOURCE.GetResource<Mesh>("Sphere");

		//// [최적화] 모든 구가 공유할 기본 텍스처들 (White)
		//// 텍스처 값(1.0) * 팩터 값(설정값) = 최종 값
		//TexturePtr sharedWhite = Texture::CreateWhite();

		//const int rows = 7;
		//const int cols = 7;
		//const float spacing = 1.4f;

		//for (int row = 0; row < rows; ++row)
		//{
		//	// Metallic (0.0 ~ 1.0)
		//	float metallicValue = (float)row / (float)(rows - 1);
		//	float y = ((float)row - (float)(rows - 1) * 0.5f) * spacing;

		//	for (int col = 0; col < cols; ++col)
		//	{
		//		// Roughness (0.05 ~ 1.0)
		//		float roughnessValue = (float)col / (float)(cols - 1);
		//		roughnessValue = glm::clamp(roughnessValue, 0.05f, 1.0f);

		//		float x = ((float)col - (float)(cols - 1) * 0.5f) * spacing;

		//		auto sphereObj = GameObject::Create();
		//		sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

		//		// 머티리얼 생성
		//		MaterialPtr pbrMat = Material::Create();

		//		// 1. 텍스처는 모두 White로 통일 (공유)
		//		pbrMat->diffuse = sharedWhite;
		//		pbrMat->metallic = sharedWhite;
		//		pbrMat->roughness = sharedWhite;
		//		pbrMat->ao = sharedWhite;
		//		// Normal이 없으면 SetToProgram에서 자동으로 Blue(Flat)가 바인딩되므로 생략 가능

		//		// 2. Factor에 값을 설정 [핵심!]
		//		pbrMat->albedoFactor = glm::vec4(0.2f, 0.4f, 0.5f, 1.0f); // 빨간공
		//		pbrMat->metallicFactor = metallicValue;   // 팩터로 조절
		//		pbrMat->roughnessFactor = roughnessValue; // 팩터로 조절

		//		// 렌더러 등록
		//		auto mr = MeshRenderer::Create(sphereMesh, pbrMat);
		//		hdrPass->AddRenderer(mr.get());
		//		sphereObj->AddComponent(std::move(mr));
		//		AddGameObject(std::move(sphereObj));
		//	}
		//}
	}

	return true;
}