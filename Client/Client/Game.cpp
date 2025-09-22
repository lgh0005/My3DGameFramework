#include "pch.h"
#include "Game.h"

int SDL_main(int, char**)
{
	// Game content scenes
	vector<shared_ptr<Scene>> SCENES
	{
		make_shared<DeviceInit>("DeviceInit")
	};

	// Run game application
	LAUNCH->AddScenes(SCENES);
	LAUNCH->Run("DeviceInit");

	return 0;
}