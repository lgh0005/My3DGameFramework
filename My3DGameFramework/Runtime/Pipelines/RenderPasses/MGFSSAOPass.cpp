#include "FrameworkPch.h"
#include "MGFSSAOPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"

namespace MGF3D
{
	MGFSSAOPass::MGFSSAOPass() = default;
	MGFSSAOPass::~MGFSSAOPass() = default;

	MGFSSAOPassUPtr MGFSSAOPass::Create()
	{
		return nullptr;
	}

	bool MGFSSAOPass::Init()
	{
		auto vs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Lighting.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Deferred_Lighting.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		// m_skyboxProgram = MGF_RESOURCE.Create<GraphicsProgram>("SkyboxProgram", Vector<ShaderPtr>{ vs, fs });
		return true;
	}

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 MGFSSAOPass::s_typeIndex = -1;
	const MGFType* MGFSSAOPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}