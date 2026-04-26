#include "RuntimePch.h"
#include "TestRenderPass.h"
#include "Managers/TypeManager.h"
#include "Managers/AssetManager.h"
#include "Graphics/Programs/GraphicsProgram.h"

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
		m_program = GraphicsProgram::Create("TestProgram");
		//auto vs = MGF_ASSET.Get<Shader>("Shaders/Test_VS.glsl");
		//auto fs = MGF_ASSET.Get<Shader>("Shaders/Test_FS.glsl");
		//m_program->AddShader(vs);
		//m_program->AddShader(fs);
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

	}
}