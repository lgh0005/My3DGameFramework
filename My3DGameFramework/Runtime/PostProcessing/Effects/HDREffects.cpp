#include "RuntimePch.h"
#include "HDREffects.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h"
#include "Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"

namespace MGF3D
{
	HDREffects::HDREffects() = default;
	HDREffects::~HDREffects() = default;

	HDREffectsUPtr HDREffects::Create()
	{
		auto effect = HDREffectsUPtr(new HDREffects());
		if (!effect->Init()) return nullptr;
		return effect;
	}

	bool HDREffects::Init()
	{
		// 1. HDR 효과용 프로그램 로드
		auto hdrVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/PostProcessing/MGF3D_PostProcess_HDR.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto hdrFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/PostProcessing/MGF3D_PostProcess_HDR.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_highDynamicRangeProgram = MGF_RESOURCE.Create<GraphicsProgram>("HighDynamicRangeProgram", Vector<ShaderPtr>{ hdrVs, hdrFs });

		return true;
	}

	void HDREffects::Resize(int32 width, int32 height)
	{

	}

	bool HDREffects::Render(RenderContext* context)
	{
		return true;
	}
}
