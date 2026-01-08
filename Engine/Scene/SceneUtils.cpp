#include "EnginePch.h"
#include "SceneUtils.h"
#include "Scene/Scene.h"
#include "GameObject/GameObject.h"

Scene* SceneUtils::GetCurrentScene()
{
	return SCENE.GetActiveScene();
}

GameObject* SceneUtils::FindObject(const std::string& name)
{
	Scene* scene = GetCurrentScene();
	if (!scene) return nullptr;

	return scene->FindGameObject(name);
}

void SceneUtils::Destroy(GameObject* obj)
{
	if (!obj) return;

	Scene* scene = GetCurrentScene();
	if (!scene) return;

	scene->Destroy(obj);
}
