#include "CorePch.h"
#include "CoreModule.h"

#include "Managers/WindowManager.h"
#include "Managers/TimeManager.h"
#include "Managers/InputManager.h"
#include "Managers/TypeManager.h"

#include "Identities/MGFTypeTree.h"

#include "Input/MGFInputDevice.h"
#include "Input/Devices/MGFKeyboardDevice.h"
#include "Input/Devices/MGFMouseDevice.h"

namespace MGF3D
{
	void CoreModule::OnRegisterTypes()
	{
		// 1. InputModule이 "Device" 도메인 트리의 생성을 주도
		MGFTypeTree& deviceTree = MGF_TYPE.CreateTree("Device");
		MGFInputDevice::s_typeIndex = deviceTree.Register("MGFInputDevice", "");
		MGFKeyboardDevice::s_typeIndex = deviceTree.Register("MGFKeyboardDevice", "MGFInputDevice");
		MGFMouseDevice::s_typeIndex = deviceTree.Register("MGFMouseDevice", "MGFInputDevice");
		MGF_TYPE.BakeAll();

		int a = 10;
		int b = 20;
	}

	bool CoreModule::OnInit()
	{
		MGF_LOG_INIT();

		// 1. 윈도우 매니저 초기화
		if (!MGF_WINDOW.Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, 0))
		{
			MGF_LOG_FATAL("Failed to initialize WindowManager.");
			return false;
		}

		// 2. 인풋 매니저 초기화
		if (!MGF_INPUT.Init(MGF_WINDOW.GetNativeHandle()))
		{
			MGF_LOG_FATAL("Failed to initialize InputManager.");
			return false;
		}

		// 3. 타임 매니저
		MGF_TIME.Init();
	}

	bool CoreModule::OnShutdown()
	{
		MGF_INPUT.Shutdown();
		MGF_WINDOW.Shutdown();

		MGF_LOG_FLUSH();
		MGF_LOG_CLEAR();

		return true;
	}
}