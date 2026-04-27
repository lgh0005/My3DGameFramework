#include "RuntimePch.h"
#include "TestRenderPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"

namespace MGF3D
{
	TestRenderPass::TestRenderPass() = default;
	TestRenderPass::~TestRenderPass() = default;

	TestRenderPassUPtr TestRenderPass::Create()
	{
		auto renderPass = TestRenderPassUPtr(new TestRenderPass());
		if (!renderPass->Init()) return nullptr;
		return renderPass;
	}

	bool TestRenderPass::Init()
	{
		m_program = MGF_RESOURCE.GetOrCreate<GraphicsProgram>("TestDebugProgram");
		auto vs = MGF_ASSET.LoadAssetAsync<Shader>("@GameAsset/Test/Test.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs = MGF_ASSET.LoadAssetAsync<Shader>("@GameAsset/Test/Test.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_program->AddShader(vs);
		m_program->AddShader(fs);
		m_program->SetState(EResourceState::Loaded);
		return true;
	}	

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 TestRenderPass::s_typeIndex = -1;
	const MGFType* TestRenderPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("TestRenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void TestRenderPass::Execute(RenderContext* context)
	{
		if (!context || !m_program) return;

		// 1. 화면 초기화 (Background Color 설정)
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. 렌더링 상태 설정 (Depth Test 등)
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// 1. 프로그램 리소스가 Ready 상태인지 체크
		if (m_program->GetState() != EResourceState::Ready)
		{
			if (!m_program->OnSyncCreate()) return;
			MGF_LOG_INFO("TestRenderPass: Program is linked and ready.");
		}

		// 3. 프로그램 사용 (glUseProgram)
		m_program->Use();
	
		// 1. 카메라의 위치와 주시점 설정
		vec3 eye = vec3(2.0f, 0.0f, 5.0f);  // 카메라를 +5 지점으로 옮깁니다. (안전하게)
		vec3 center = vec3(0.0f, 0.0f, 0.0f);  // 원점(큐브)을 바라봅니다.
		vec3 up     = vec3(0.0f, 1.0f, 0.0f);  // 위쪽 방향
		mat4 view = glm::lookAt(eye, center, up);
		mat4 proj = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		mat4 viewProj = proj * view;
		m_program->SetUniform("uViewProj", viewProj);

		// 4. 큐 실행
		// 정적 메쉬 렌더링 (Slot 1 사용)
		context->GetStaticQueue().Execute();

		// 스킨드 메쉬 렌더링 (동일하게 Slot 1 사용하지만 배치가 다르므로 문제 없음)
		context->GetSkinnedQueue().Execute();
	}
}