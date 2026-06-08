#include "RuntimePch.h"
#include "MGFGeometryPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Managers/WindowManager.h"
#include "Managers/EntityManager.h"
#include "Managers/AnimationManager.h"
#include "Instancing/Animations/AnimationBuffer.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"
#include "Framebuffers/GLFramebuffer2D.h"

namespace MGF3D
{
	MGFGeometryPass::MGFGeometryPass() = default;
	MGFGeometryPass::~MGFGeometryPass() = default;

	MGFGeometryPassUPtr MGFGeometryPass::Create()
	{
		auto renderPass = MGFGeometryPassUPtr(new MGFGeometryPass());
		if (!renderPass->Init()) return nullptr;
		return renderPass;
	}

	bool MGFGeometryPass::Init()
	{
		auto vs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Geometry_Static.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Geometry.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_geometryStaticDrawProgram = MGF_RESOURCE.Create<GraphicsProgram>("GeometryStaticDrawProgram", Vector<ShaderPtr>{ vs, fs });

		auto vs2 = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Geometry_Skinned.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs2 = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Geometry.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_geometrySkinnedDrawProgram = MGF_RESOURCE.GetOrCreate<GraphicsProgram>("GeometrySkinnedDrawProgram", Vector<ShaderPtr>{ vs2, fs2 });

		return true;
	}

	void MGFGeometryPass::Resize()
	{

	}

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 MGFGeometryPass::s_typeIndex = -1;
	const MGFType* MGFGeometryPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFGeometryPass::Execute(RenderContext* context)
	{
		if (!context || !m_geometryStaticDrawProgram || !m_geometrySkinnedDrawProgram) return;
		if (m_geometryStaticDrawProgram->GetState() != EResourceState::Ready ||
			m_geometrySkinnedDrawProgram->GetState() != EResourceState::Ready) return;

		// 2. G-Buffer 프레임버퍼 가져오기
		auto gBuffer = context->GetGeometryBuffer();
		if (!gBuffer) return;

		// 3. G-Buffer 바인딩 및 화면 클리어
		gBuffer->Bind();
		glViewport(0, 0, gBuffer->GetWidth(), gBuffer->GetHeight());

		// 배경을 투명한 검은색으로 클리어 (Albedo alpha 등이 0이 되도록)
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 4. 지오메트리 렌더링 상태 설정
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);

		// 3. 정적 메쉬 렌더링
		m_geometryStaticDrawProgram->Use();
		context->GetStaticQueue().Execute();

		// 4. 스킨드 메쉬 렌더링
		m_geometrySkinnedDrawProgram->Use();
		MGF_ANIM.GetAnimationBuffer()->Bind(9);
		context->GetSkinnedQueue().Execute();

		// 5. G-Buffer 바인딩 해제
		gBuffer->Unbind();
	}
}