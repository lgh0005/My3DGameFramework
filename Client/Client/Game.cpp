#include "pch.h"
#include "Game.h"

int SDL_main(int, char**)
{
	// Game content scenes
	vector<shared_ptr<Scene>> SCENES
	{
		make_shared<DeviceInit>("DeviceInit"),
		make_shared<Triangle>("Triangle"),
		make_shared<Rectangle>("Rectangle")
	};

	// Run game application
	LAUNCH->AddScenes(SCENES);
	LAUNCH->Run("Rectangle");

	return 0;
}