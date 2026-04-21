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
	StaticMeshPtr      Application::g_debugCube = nullptr;
	GraphicsProgramPtr Application::g_debugProgram = nullptr;
	ShaderPtr		  Application::g_debugVS = nullptr;
	ShaderPtr		  Application::g_debugFS = nullptr;

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

		// 2. RuntimeModule 초기화
		if (!RuntimeModule::OnInit()) return false;

		// [DEBUG] 비동기 파이프라인 가동
		{
			// 1. 기본 큐브 메쉬 생성 (이미 동기적으로 Ready 상태가 됨)
			g_debugCube = GeometryUtils::CreateBox();

			// 2. 셰이더 에셋 비동기 로드 요청 (상태: Loading)
			g_debugVS = MGF_ASSET.LoadAssetAsync<Shader>("@GameAsset/Test/Test.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
			g_debugFS = MGF_ASSET.LoadAssetAsync<Shader>("@GameAsset/Test/Test.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);

			// 3. 그래픽스 프로그램 리소스 생성 (상태: Loaded)
			// NamedResource이므로 GetOrCreate 사용
			g_debugProgram = MGF_RESOURCE.GetOrCreate<GraphicsProgram>("TestDebugProgram");

			// 4. 의존성 연결
			// 각 에셋이 가진 리소스(GLShader)를 프로그램에 등록합니다.
			// 이 리소스들은 아직 Ready가 아닐 수 있지만, Program::OnSyncCreate에서 걸러낼 것입니다.
			if (g_debugProgram)
			{
				g_debugProgram->AddShader(g_debugVS);
				g_debugProgram->AddShader(g_debugFS);
				g_debugProgram->SetState(EResourceState::Loaded);
			}

			// 이제 모든 것은 GPU 워커 큐의 GPUSyncTask가 해결해 줄 것입니다.
			glEnable(GL_DEPTH_TEST);
		}

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

			// 3. [DEBUG] 렌더링 테스트
			{
				glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// 위에서 호출한 Update들 덕분에 상태가 Ready로 변경되면 여기서 바로 그려집니다.
				if (g_debugProgram && g_debugProgram->GetState() == EResourceState::Ready &&
					g_debugCube && g_debugCube->GetState() == EResourceState::Ready)
				{
					g_debugProgram->Use();

					float time = (float)glfwGetTime();
					float aspect = (float)MGF_WINDOW.GetWindowWidth() / (float)MGF_WINDOW.GetWindowHeight();

					glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
					glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
					glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
					glm::mat4 mvp = proj * view * model;

					g_debugProgram->SetUniform("uMVP", mvp);
					g_debugProgram->SetUniform("uColor", vec3(1.0f, 0.5f, 0.2f));

					g_debugCube->Draw(1);
				}
			}

			MGF_SCENE.Update();
			MGF_TIME.FixedUpdate();
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