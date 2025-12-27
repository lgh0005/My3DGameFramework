#include "pch.h"
#include "My3DGameFramework.h"

int main()
{
	// 1. 어플리케이션(엔진) 인스턴스 생성
	auto app = My3DGameFramework::Create
	(
		WINDOW_WIDTH, WINDOW_HEIGHT,
		WINDOW_NAME
	);
	if (!app)
	{
		SPDLOG_CRITICAL("failed to create application instance.");
		return 1;
	}

	// 2. 게임 실행
	// Now working: DevLevel, PBRLevel
	app->Run("PBRLevel");

	return 0;
}