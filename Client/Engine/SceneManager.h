#pragma once
#include "ISingleton.h"

class Scene;

class SceneManager : public ISingleton<SceneManager>
{
	DECLARE_SINGLE(SceneManager);

public:
	virtual ~SceneManager() override = default;

public:
	virtual void Init() override;
	virtual void Update() override;

public:
	void AddScenes(const vector<shared_ptr<Scene>>& scenes);
	void LoadScene(const string& scene);

private:
	unordered_map<string, shared_ptr<Scene>> _scenes;
	shared_ptr<Scene> _currentScene;
};

