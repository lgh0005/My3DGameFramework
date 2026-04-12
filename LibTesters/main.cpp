#include "pch.h"
#include "Application/Application.h"
using namespace MGF3D;

int main()
{
	// 게임 엔진 프레임워크 초기화
	if (!Application::Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, 0))
		return -1;

	// 게임 엔진 메인 루프
	Application::Run();

	// 게임 엔진 종료
	if (!Application::Shutdown())
		return -1;

	return 0;
}