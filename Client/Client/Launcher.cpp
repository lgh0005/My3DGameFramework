#include "pch.h"
#include "Launcher.h"

unique_ptr<Launcher> LAUNCH = make_unique<Launcher>();

void Launcher::AddScenes(const vector<shared_ptr<Scene>>& scenes)
{
	SCENE.AddScenes(scenes);
}

void Launcher::Run(const string& FirstSceneName)
{
	GAME->Init();
	GAME->Launch(FirstSceneName);
}
