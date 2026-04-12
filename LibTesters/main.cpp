#include "GamePch.h"
#include "Application/Application.h"

int main()
{
	// 게임 엔진 코어 타입 호적 등록 및 레벨 등록
	MGF3D::Application::RegisterTypes();

	// 게임 엔진 프레임워크 초기화
	if (!MGF3D::Application::Init())
		return -1;

	// 게임 엔진 메인 루프
	MGF3D::Application::Run();

	// 게임 엔진 종료
	if (!MGF3D::Application::Shutdown())
		return -1;

	return 0;
}