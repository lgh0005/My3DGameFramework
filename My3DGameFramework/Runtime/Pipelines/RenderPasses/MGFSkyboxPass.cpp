#include "RuntimePch.h"
#include "MGFSkyboxPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"

namespace MGF3D
{
	MGFSkyboxPass::MGFSkyboxPass() = default;
	MGFSkyboxPass::~MGFSkyboxPass() = default;

	MGFSkyboxPassUPtr MGFSkyboxPass::Create()
	{
		auto renderPass = MGFSkyboxPassUPtr(new MGFSkyboxPass());
		if (!renderPass->Init()) return nullptr;
		return renderPass;
	}

	bool MGFSkyboxPass::Init()
	{
		auto vs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Lighting.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Lighting.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_skyboxProgram = MGF_RESOURCE.Create<GraphicsProgram>("SkyboxProgram", Vector<ShaderPtr>{ vs, fs });
		return true;
	}

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 MGFSkyboxPass::s_typeIndex = -1;
	const MGFType* MGFSkyboxPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFSkyboxPass::Execute(RenderContext* context)
	{
		// TODO : 절두체 기반 컬링
	}
}