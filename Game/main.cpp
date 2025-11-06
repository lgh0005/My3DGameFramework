#include "pch.h"
#include "Game.h"

GameUPtr g_Game;

int main()
{
	// 1. 게임 프로그램 시작
	g_Game = Game::Create();
	if (!g_Game)
	{
		SPDLOG_ERROR("failed to create Game.");
		return 1;
	}

	// 2. 게임 로직 수행
	g_Game->Update();

	// 3. 게임 종료
	g_Game->Shutdown();

	return 0;
}