#include "../pch.h"
#include "DevScene.h"

#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Image.h"
#include "Components/Camera.h"
#include "Core/GameObject.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"

DevSceneUPtr DevScene::Create()
{
	auto devScene = DevSceneUPtr(new DevScene());
	if (!devScene->Init()) return nullptr;
	return std::move(devScene);
}

bool DevScene::Init()
{
	// 3. 카메라 GameObject 생성
	{
		// GameObject 생성 및 씬에 등록
		auto cameraObj = GameObject::Create(); if (!cameraObj) return false;
		auto camera = Camera::Create();		   if (!camera)	   return false;
		auto* cameraPtr = camera.get();

		cameraObj->GetTransform().SetPosition(glm::vec3(0.0f, 2.5f, 8.0f));
		camera->SetProjection(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 100.0f);
		cameraObj->AddComponent(std::move(camera));

		SetActiveCamera(cameraPtr);

		AddGameObject(std::move(cameraObj));
	}

	// 4. 큐브 생성
	{
		auto boxMesh = Mesh::CreateBox();	   if (!boxMesh) return false;
		TextureUPtr diffuseTexture = Texture::CreateFromImage
		(Image::Load("./Resources/Images/container.jpg").get());
		TextureUPtr specularTexture = Texture::CreateFromImage
		(Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
		if (!diffuseTexture || !specularTexture) return false;

		auto box1Mat = Material::Create();
		box1Mat->diffuse = std::move(diffuseTexture);
		box1Mat->specular = std::move(specularTexture);
		box1Mat->shininess = 16.0f;

		auto cubeObj = GameObject::Create();
		cubeObj->SetName("Box1");
		auto& cubeTransform = cubeObj->GetTransform();
		cubeTransform.SetPosition(glm::vec3(-1.0f, 0.75f, -5.0f));
		cubeTransform.SetRotation(glm::vec3(0.0f, 30.0f, 0.0f));
		cubeTransform.SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

		auto meshRenderer = MeshRenderer::Create(std::move(boxMesh), std::move(box1Mat));
		cubeObj->AddComponent(std::move(meshRenderer));

		AddGameObject(std::move(cubeObj));
	}

	return true;
}
