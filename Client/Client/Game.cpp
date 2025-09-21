#include "pch.h"
#include "Game.h"

int SDL_main(int, char**)
{
	GAME->Init();
	GAME->Launch();
	return 0;
}