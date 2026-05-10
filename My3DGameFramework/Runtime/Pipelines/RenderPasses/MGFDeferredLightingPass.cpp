#include "RuntimePch.h"
#include "MGFDeferredLightingPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"
#include "Textures/GLTexture2D.h"
#include "Meshes/ScreenMesh.h"

namespace MGF3D
{
	MGFDeferredLightingPass::MGFDeferredLightingPass() = default;
	MGFDeferredLightingPass::~MGFDeferredLightingPass() = default;

	MGFDeferredLightingPassUPtr MGFDeferredLightingPass::Create()
	{
		auto renderPass = MGFDeferredLightingPassUPtr(new MGFDeferredLightingPass());
		if (!renderPass->Init()) return nullptr;
		return renderPass;
	}

	bool MGFDeferredLightingPass::Init()
	{
		auto vs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Lighting.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Lighting.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_deferredLightingProgram = MGF_RESOURCE.Create<GraphicsProgram>("DeferredLightingProgram", Vector<ShaderPtr>{ vs, fs });
		return true;
	}

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 MGFDeferredLightingPass::s_typeIndex = -1;
	const MGFType* MGFDeferredLightingPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFDeferredLightingPass::Execute(RenderContext* context)
	{
		if (!context || !m_deferredLightingProgram) return;

		if (m_deferredLightingProgram->GetState() != EResourceState::Ready) return;

		// 2. 화면 초기화 (조명 결과가 그려질 기본 프레임버퍼)
		glClearColor(0.55f, 0.45, 0.95f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 3. 디퍼드 라이팅은 화면에 2D 사각형을 그리는 작업이므로 깊이 테스트 비활성화
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		// 4. G-Buffer 텍스처들을 가져와 셰이더의 각 슬롯(binding = 0 ~ 3)에 바인딩
		auto posAOTex = context->GetGeometryBufferTexture(EGBufferSlot::PositionAO);
		auto normRoughTex = context->GetGeometryBufferTexture(EGBufferSlot::NormalRoughness);
		auto albMetalTex = context->GetGeometryBufferTexture(EGBufferSlot::AlbedoMetallic);
		auto emissionTex = context->GetGeometryBufferTexture(EGBufferSlot::Emission);

		if (posAOTex)     posAOTex->Bind(0);
		if (normRoughTex) normRoughTex->Bind(1);
		if (albMetalTex)  albMetalTex->Bind(2);
		if (emissionTex)  emissionTex->Bind(3);

		// 5. 셰이더 활성화
		m_deferredLightingProgram->Use();
		context->GetScreenMesh()->Draw();
	}
}