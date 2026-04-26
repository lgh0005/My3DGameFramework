#include "CorePch.h"
#include "Application.h"

#pragma region MODULES
#include "CoreModule.h"
#include "GraphicsModule.h"
#include "FrameworkModule.h"
#include "RuntimeModule.h"
#pragma endregion

#pragma region MANAGERS
#include "Managers/InputManager.h"
#include "Managers/WindowManager.h"
#include "Managers/TimeManager.h"
#include "Managers/SceneManager.h"
#include "Managers/TypeManager.h"
#include "Managers/AssetManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/RenderManager.h"
#pragma endregion

#pragma region TEMP
#include "Input/Devices/MGFKeyboardDevice.h"
#include "Input/Devices/MGFMouseDevice.h"
#pragma endregion

#pragma region ASSET_RESOURCE_DEBUG
#include "Assets/Shader.h"
#include "Programs/GraphicsProgram.h"
#include "Meshes/StaticMesh.h"
#include "Shader/GLShader.h"
#include "GraphicsUtils/GeometryUtils.h"
#pragma endregion

namespace MGF3D
{
	void Application::RegisterTypes()
	{
		CoreModule::OnRegisterTypes();
		GraphicsModule::OnRegisterTypes();
		FrameworkModule::OnRegisterTypes();
		RuntimeModule::OnRegisterTypes();
	}

	bool Application::Init()
	{
		// 0. CoreModule 초기화
		if (!CoreModule::OnInit()) return false;

		// 0-1. [DEBUG] 디바이스 가져오기 및 키 매핑
		{
			auto* kbd = MGF_INPUT.GetDevice<MGFKeyboardDevice>(); if (!kbd) return -1;
			kbd->MapKey("Jump", GLFW_KEY_SPACE);

			auto* mouse = MGF_INPUT.GetDevice<MGFMouseDevice>(); if (!mouse) return -1;
			mouse->MapButton("Fire", GLFW_MOUSE_BUTTON_LEFT);
		}

		// 1. SystemModule 초기화
		if (!GraphicsModule::OnInit()) return false;

		// 2. FrameworkModule 초기화
		if (!FrameworkModule::OnInit()) return false;

		// 3. RuntimeModule 초기화
		if (!RuntimeModule::OnInit()) return false;

		return true;
	}

	void Application::Run()
	{
		while (!MGF_WINDOW.ShouldClose())
		{
			MGF_TIME.Update();
			MGF_WINDOW.Update();
			MGF_RESOURCE.Update();
			MGF_ASSET.Update();

			// 2. [DEBUG] 액션 기반 로직 처리
			{
				MGF_INPUT.Update(MGF_WINDOW.GetNativeHandle());
				if (MGF_INPUT.GetDevice<MGFKeyboardDevice>()->GetButtonDown("Jump")) MGF_LOG_INFO("Action: Jump!");
				if (MGF_INPUT.GetDevice<MGFMouseDevice>()->GetButtonDown("Fire"))  MGF_LOG_INFO("Action: Fire!");
			}

			MGF_ENTITY.Update();
			MGF_SCENE.Update();
			MGF_TIME.FixedUpdate();
			MGF_RENDER.Render();
			MGF_WINDOW.SwapWindowBuffers();
		}
	}

	bool Application::Shutdown()
	{
		// 0. RuntimeModule 종료
		if (!RuntimeModule::OnShutdown()) return false;

		// 0. FrameworkModule 종료
		if (!FrameworkModule::OnShutdown()) return false;

		// 1. GraphicsModule 종료
		if (!GraphicsModule::OnShutdown()) return false;

		// 2. CoreModule 종료
		if (!CoreModule::OnShutdown()) return false;

		return true;
	}
}