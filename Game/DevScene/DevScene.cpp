#include "pch.h"
#include "DevScene.h"

#include "Core/GameObject.h"
#include "Core/RenderPass.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/SkinnedMesh.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Image.h"
#include "Graphics/Animation.h"
#include "Graphics/Model.h"
#include "Graphics/Buffer.h"
#include "Graphics/InstancedMesh.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"

#include "RenderPasses/StaticRenderPass.h"
#include "RenderPasses/SimpleRenderPass.h"
#include "RenderPasses/SkinningRenderPass.h"
#include "RenderPasses/InstancedRenderPass.h"

DevScene::~DevScene() = default;

DevSceneUPtr DevScene::Create()
{
	auto devScene = DevSceneUPtr(new DevScene());
	if (!devScene->Init()) return nullptr;
	return std::move(devScene);
}

bool DevScene::CreateNessesaryRenderPasses()
{
	// 1. StaticMesh 셰이더 (조명 O)
	{
		auto prog = Program::Create(
			"./Resources/Shaders/lighting2.vert",
			"./Resources/Shaders/lighting2.frag");
		if (!prog) return false;
		AddRenderPass("Static", StaticRenderPass::Create(std::move(prog)));
	}

	// 2. SkinnedMesh 셰이더 (조명 O)
	{
		auto prog = Program::Create(
			"./Resources/Shaders/skinningLight.vert",
			"./Resources/Shaders/skinningLight.frag");
		if (!prog) return false;
		AddRenderPass("Skinned", SkinningRenderPass::Create(std::move(prog)));
	}

	// 3. Instanced 셰이더 (잔디)
	{
		auto prog = Program::Create(
			"./Resources/Shaders/Instancing/grass.vert",
			"./Resources/Shaders/Instancing/grass.frag");
		if (!prog) return false;
		AddRenderPass("Instanced", InstancedRenderPass::Create(std::move(prog)));
	}

	// 4. Simple 셰이더 (조명 기즈모)
	{
		auto prog = Program::Create(
			"./Resources/Shaders/simple.vert",
			"./Resources/Shaders/simple.frag");
		if (!prog) return false;
		AddRenderPass("LightGizmo", SimpleRenderPass::Create(std::move(prog)));
	}

	return true;
}

bool DevScene::LoadNessesaryResources()
{
	// 0-1. 큐브 메쉬
	auto boxMesh = StaticMesh::CreateBox();
	RESOURCE.AddResource<Mesh>("Cube", std::move(boxMesh));

	// 0-2. 모델과 애니메이션
	auto model = Model::Load("./Resources/Models/spacesoldier/aliensoldier.mymodel");
	auto anim = Animation::Load("./Resources/Models/spacesoldier/Idle.fbx", model.get());
	RESOURCE.AddResource<Animation>("hiphopDancing", std::move(anim));
	RESOURCE.AddResource<Model>("aliensoldier", std::move(model));

	// 0-3. 머티리얼 1
	{
		TextureUPtr diffuseTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());
		TextureUPtr specularTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
		if (!diffuseTexture || !specularTexture) return false;

		auto lightMat = Material::Create();
		lightMat->diffuse = std::move(diffuseTexture);
		lightMat->specular = std::move(specularTexture);
		lightMat->shininess = 16.0f;
		RESOURCE.AddResource<Material>("LightMat", std::move(lightMat));
	}

	// 0-4. 머티리얼 2
	{
		TextureUPtr diffuseTexture = Texture::CreateFromImage
		(Image::Load("./Resources/Images/container.jpg").get());
		TextureUPtr specularTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
		if (!diffuseTexture || !specularTexture) return false;

		auto box1Mat = Material::Create();
		box1Mat->diffuse = std::move(diffuseTexture);
		box1Mat->specular = std::move(specularTexture);
		box1Mat->shininess = 16.0f;
		RESOURCE.AddResource<Material>("boxMat1", std::move(box1Mat));
	}

	// 0-5. 머티리얼 3
	{
		TextureUPtr diffuseTexture = Texture::CreateFromImage
		(Image::Load("./Resources/Images/container2.png").get());
		TextureUPtr specularTexture = Texture::CreateFromImage
		(Image::Load("./Resources/Images/container2_specular.png").get());
		if (!diffuseTexture || !specularTexture) return false;

		auto box2Mat = Material::Create();
		box2Mat->diffuse = std::move(diffuseTexture);
		box2Mat->specular = std::move(specularTexture);
		box2Mat->shininess = 16.0f;
		RESOURCE.AddResource<Material>("boxMat2", std::move(box2Mat));
	}

	// 0-6. 머티리얼 4
	{
		TextureUPtr diffuseTexture = Texture::CreateFromImage
		(Image::Load("./Resources/Images/marble.jpg").get());
		TextureUPtr specularTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(0.25f, 0.25f, 0.25f, 1.0f)).get());
		if (!diffuseTexture || !specularTexture) return false;

		auto box4Mat = Material::Create();
		box4Mat->diffuse = std::move(diffuseTexture);
		box4Mat->specular = std::move(specularTexture);
		box4Mat->shininess = 16.0f;
		RESOURCE.AddResource<Material>("boxMat3", std::move(box4Mat));
	}

	// 0-7. 풀떼기
	{
		auto grassTexture = Texture::CreateFromImage
		(Image::Load("./Resources/Images/grass.png").get());
		if (!grassTexture) return false;

		auto grassMat = Material::Create();
		grassMat->diffuse = std::move(grassTexture);
		RESOURCE.AddResource<Material>("grassMat", std::move(grassMat));

		auto bladeMesh = StaticMesh::CreatePlane();
		RESOURCE.AddResource<Mesh>("grassBlade", std::move(bladeMesh));
	}

	return true;
}

bool DevScene::CreateSceneContext()
{
	RenderPass* lightPass = GetRenderPass("LightGizmo");
	RenderPass* staticPass = GetRenderPass("Static");
	RenderPass* skinnedPass = GetRenderPass("Skinned");
	RenderPass* grassPass = GetRenderPass("Instanced");

	// 3. 카메라 GameObject 생성
	{
		// GameObject 생성 및 씬에 등록
		auto cameraObj = GameObject::Create(); if (!cameraObj) return false;
		auto camera = Camera::Create();		   if (!camera)	   return false;
		auto* cameraPtr = camera.get();

		cameraObj->GetTransform().SetPosition(glm::vec3(0.0f, 2.5f, 8.0f));
		camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 100.0f);
		cameraObj->AddComponent(std::move(camera));

		SetMainCamera(cameraPtr);

		AddGameObject(std::move(cameraObj));
	}

	// 조명 큐브
	{
		auto lightGo = GameObject::Create();
		lightGo->SetName("SpotLight");

		auto lightComp = SpotLight::Create();
		SetMainLight(lightComp.get());
		lightGo->GetTransform().SetPosition(glm::vec3(1.0f, 4.0f, 4.0f));
		lightGo->GetTransform().SetScale(glm::vec3(0.2f));
		lightComp->SetCutoff(glm::vec2(120.0f, 5.0f));
		lightComp->SetDistance(128.0f);
		lightGo->AddComponent(std::move(lightComp));
		auto renderer = MeshRenderer::Create
		(RESOURCE.GetResource<Mesh>("Cube"), RESOURCE.GetResource<Material>("LightMat"));
		lightPass->AddRenderer(renderer.get());
		lightGo->AddComponent(std::move(renderer));
		AddGameObject(std::move(lightGo));
	}

	// 4. 큐브 생성 #1
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Box1");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(3.0f, 2.75f, -4.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		auto meshRenderer = MeshRenderer::Create
		(RESOURCE.GetResource<Mesh>("Cube"), RESOURCE.GetResource<Material>("boxMat1"));
		staticPass->AddRenderer(meshRenderer.get());
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 5. 큐브 생성 #2
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Box2");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(0.0f, 0.75f, 2.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 20.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		auto meshRenderer = MeshRenderer::Create
		(RESOURCE.GetResource<Mesh>("Cube"), RESOURCE.GetResource<Material>("boxMat2"));
		staticPass->AddRenderer(meshRenderer.get());
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 5. 큐브 생성 #3
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Box2");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(-1.0f, 0.75f, -4.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		auto meshRenderer = MeshRenderer::Create
		(RESOURCE.GetResource<Mesh>("Cube"), RESOURCE.GetResource<Material>("boxMat2"));
		staticPass->AddRenderer(meshRenderer.get());
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 6. 큐브 생성 #3
	{
		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Ground");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
		cubeTransform.SetScale(glm::vec3(10.0f, 1.0f, 10.0f));

		auto meshRenderer = MeshRenderer::Create
		(RESOURCE.GetResource<Mesh>("Cube"), RESOURCE.GetResource<Material>("boxMat3"));
		staticPass->AddRenderer(meshRenderer.get());
		cubeObj->AddComponent(std::move(meshRenderer));
		AddGameObject(std::move(cubeObj));
	}

	// 7. 모델
	{
		auto modelGo = GameObject::Create();
		modelGo->SetName("Soldier");
		modelGo->GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, -2.0f));
		modelGo->GetTransform().SetScale(glm::vec3(0.025f));

		// 3. GameObject에 Animator 컴포넌트 추가
		modelGo->AddComponent(Animator::Create(RESOURCE.GetResource<Animation>("hiphopDancing")));

		// 4. Model 안의 모든 Mesh 조각을 MeshRenderer 컴포넌트로 추가
		auto model = RESOURCE.GetResource<Model>("aliensoldier");
		for (uint32 i = 0; i < model->GetMeshCount(); ++i)
		{
			SkinnedMeshPtr mesh = model->GetMesh(i);
			auto renderer = MeshRenderer::Create(mesh, mesh->GetMaterial());
			skinnedPass->AddRenderer(renderer.get());
			modelGo->AddComponent((std::move(renderer)));
		}

		// 5. 씬에 GameObject 등록
		AddGameObject(std::move(modelGo));
	}

	// 8. 풀떼기
	{
		int32 grassCount = 10000;
		auto bladeMesh = std::static_pointer_cast<StaticMesh>
			(RESOURCE.GetResource<Mesh>("grassBlade"));
		auto grassMat = RESOURCE.GetResource<Material>("grassMat");
		if (!bladeMesh || !grassMat) return false;

		std::vector<glm::vec3> instanceData;
		instanceData.resize(grassCount);
		srand((unsigned int)time(NULL));
		for (int i = 0; i < grassCount; ++i)
		{
			float x = ((float)rand() / (float)RAND_MAX * 10.0f) - 5.0f; // -5 ~ +5 (바닥 크기)
			float z = ((float)rand() / (float)RAND_MAX * 10.0f) - 5.0f; // -5 ~ +5
			float y_rot = glm::radians((float)rand() / (float)RAND_MAX * 360.0f);
			instanceData[i] = glm::vec3(x, y_rot, z);
		}

		BufferPtr instanceBuffer = Buffer::CreateWithData
		(
			GL_ARRAY_BUFFER, GL_STATIC_DRAW,
			instanceData.data(), sizeof(glm::vec3), instanceData.size()
		);

		auto instancedGrass = InstancedMesh::Create
		(
			bladeMesh,
			instanceBuffer,
			grassCount,
			InstancedMesh::Vec3Layout()
		);
		instancedGrass->SetMaterial(grassMat);

		auto grassGo = GameObject::Create();
		grassGo->SetName("Grass_Field");

		auto renderer = MeshRenderer::Create(std::move(instancedGrass), grassMat);

		grassPass->AddRenderer(renderer.get());
		grassGo->AddComponent(std::move(renderer));
		AddGameObject(std::move(grassGo));
	}

	return true;
}
