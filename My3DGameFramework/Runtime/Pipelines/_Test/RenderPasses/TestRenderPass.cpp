#include "RuntimePch.h"
#include "TestRenderPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Managers/AnimationManager.h"
#include "Instancing/Animations/AnimationBuffer.h"
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

		m_programAnim = MGF_RESOURCE.GetOrCreate<GraphicsProgram>("TestDebugProgram2");
		auto vs2 = MGF_ASSET.LoadAssetAsync<Shader>("@GameAsset/Test/Test_skin.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs2 = MGF_ASSET.LoadAssetAsync<Shader>("@GameAsset/Test/Test.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_programAnim->AddShader(vs2);
		m_programAnim->AddShader(fs2);
		m_programAnim->SetState(EResourceState::Loaded);

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

		// 3. 정적 메쉬 렌더링
		m_program->Use();
		context->GetStaticQueue().Execute();

		// 4. 스킨드 메쉬 렌더링
		m_programAnim->Use();
		MGF_ANIM.GetAnimationBuffer()->Bind(9);
		context->GetSkinnedQueue().Execute();
	}
}