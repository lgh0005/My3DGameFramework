#include "CorePch.h"
#include "CoreModule.h"

#pragma region MANAGERS
#include "Managers/WindowManager.h"
#include "Managers/ThreadManager.h"
#include "Managers/TimeManager.h"
#include "Managers/InputManager.h"
#include "Managers/EntityManager.h"
#pragma endregion

#pragma region TYPES
#include "Managers/TypeManager.h"
#include "Identities/MGFTypeTree.h"
#pragma endregion

#pragma region DEVICES
#include "Input/MGFInputDevice.h"
#include "Input/Devices/MGFKeyboardDevice.h"
#include "Input/Devices/MGFMouseDevice.h"
#pragma endregion

#pragma region COMPONENT
#include "Entities/Component.h"
#include "Registries/ComponentRegistry.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#pragma endregion

#pragma region RESOURCE
#include "Sources/Resource.h"
#include "Sources/Asset.h"
#pragma endregion

#pragma region MISC
#include "Window/MGFWindow.h"
#pragma endregion

namespace MGF3D
{
	void CoreModule::OnRegisterTypes()
	{
		// 0. 컴포넌트 레지스트리 등록

		// 1. InputModule 계층 트리
		MGFTypeTree& deviceTree = MGF_TYPE.CreateTree("Device");
		MGFInputDevice::s_typeIndex = deviceTree.Register("MGFInputDevice", "");
		MGFKeyboardDevice::s_typeIndex = deviceTree.Register("MGFKeyboardDevice", "MGFInputDevice");
		MGFMouseDevice::s_typeIndex = deviceTree.Register("MGFMouseDevice", "MGFInputDevice");

		// 2. Component 계층 트리
		MGFTypeTree& componentTree = MGF_TYPE.CreateTree("Component");
		Component::s_typeIndex = componentTree.Register("Component", "");
		Transform::s_typeIndex = componentTree.Register("Transform", "Component");
		Camera::s_typeIndex = componentTree.Register("Camera", "Component");

		// 3. Component 레지스트리 주입
		auto transformReg = MakeUnique<ComponentRegistry<Transform>>();
		auto cameraReg = MakeUnique<ComponentRegistry<Camera>>();
		MGF_ENTITY.AddComponentRegistry(Transform::s_typeIndex, std::move(transformReg));
		MGF_ENTITY.AddComponentRegistry(Camera::s_typeIndex, std::move(cameraReg));

		// 4. Resource 트리 생성
		MGFTypeTree& resourceTree = MGF_TYPE.CreateTree("Resource");
		Resource::s_typeIndex = resourceTree.Register("Resource", "");

		// 5. Asset 트리 생성
		MGFTypeTree& assetTree = MGF_TYPE.CreateTree("Asset");
		Asset::s_typeIndex = assetTree.Register("Asset", "");
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

		// 3. 스레드 매니저
		MGF_THREAD.Init(6, 2);

		// 3. 타임 매니저
		MGF_TIME.Init();

		// 4. 엔티티 매니저
		MGF_ENTITY.Init();

		return true;
	}

	bool CoreModule::OnShutdown()
	{
		MGF_ENTITY.Shutdown();
		MGF_THREAD.Shutdown();
		MGF_INPUT.Shutdown();
		MGF_WINDOW.Shutdown();

		MGF_LOG_FLUSH();
		MGF_LOG_CLEAR();

		return true;
	}
}