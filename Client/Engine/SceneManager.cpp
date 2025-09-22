#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

void SceneManager::Init()
{
}

void SceneManager::Update()
{
	_currentScene->Update();
}

void SceneManager::AddScenes(const vector<shared_ptr<Scene>>& scenes)
{
	for (auto& scene : scenes) _scenes[scene->GetName()] = scene;
}

void SceneManager::LoadScene(const string& scene)
{
	auto nextScene = _scenes[scene];
	_currentScene = nextScene;
	_currentScene->Init();
}