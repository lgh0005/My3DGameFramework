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
		TextureUPtr diffuseTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());
		TextureUPtr specularTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
		TextureUPtr emissionTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)).get());
		TextureUPtr normalTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)).get());
		TextureUPtr heightTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());

		auto simpleMat = Material::Create();
		if (!simpleMat) return false;

		simpleMat->diffuse = std::move(diffuseTexture);
		simpleMat->specular = std::move(specularTexture);
		simpleMat->emission = std::move(emissionTexture);
		simpleMat->normal = std::move(normalTexture);
		simpleMat->height = std::move(heightTexture);
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
			"./Resources/Shaders/Universal/test_simple_shader.vert",
			"./Resources/Shaders/Universal/test_simple_shader.frag"
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


	// 2. 구 49개
	{
		// 리소스 가져오기 (LoadNessesaryResources에서 로드한 것들)
		auto sphereMesh = RESOURCE.GetResource<Mesh>("Sphere");
		auto lightMat = RESOURCE.GetResource<Material>("LightMat");

		// 7x7 그리드 생성 루프
		const int sphereCount = 7;
		const float offset = 1.4f;

		for (int row = 0; row < sphereCount; row++)
		{
			// 중앙 정렬을 위한 Y 좌표 계산
			float y = ((float)row - (float)(sphereCount - 1) * 0.5f) * offset;

			for (int col = 0; col < sphereCount; col++)
			{
				// 중앙 정렬을 위한 X 좌표 계산
				float x = ((float)col - (float)(sphereCount - 1) * 0.5f) * offset;

				// GameObject 생성
				auto sphereObj = GameObject::Create();

				// Transform 설정
				sphereObj->GetTransform().SetPosition(glm::vec3(x, y, 0.0f));

				// MeshRenderer 컴포넌트 생성
				auto mr = MeshRenderer::Create(sphereMesh, lightMat);
				mr->SetMesh(sphereMesh);
				mr->SetMaterial(lightMat);

				lightPass->AddRenderer(mr.get());

				// 컴포넌트 및 오브젝트 등록
				sphereObj->AddComponent(std::move(mr));
				AddGameObject(std::move(sphereObj));
			}
		}
	}

	return true;
}