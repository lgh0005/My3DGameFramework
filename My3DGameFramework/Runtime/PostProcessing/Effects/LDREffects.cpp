#include "RuntimePch.h"
#include "LDREffects.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Managers/TimeManager.h"
#include "Assets/Shader.h"
#include "Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"
#include "Textures/GLTexture2D.h"
#include "Meshes/ScreenMesh.h"

namespace MGF3D
{
	LDREffects::LDREffects() = default;
	LDREffects::~LDREffects() = default;

	LDREffectsUPtr LDREffects::Create()
	{
		auto effect = LDREffectsUPtr(new LDREffects());
		if (!effect->Init()) return nullptr;
		return effect;
	}

	bool LDREffects::Init()
	{
		// 1. LDR 효과용 프로그램 로드
		auto ldrVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/PostProcessing/MGF3D_PostProcess_LDR.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto ldrFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/PostProcessing/MGF3D_PostProcess_LDR.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_lowDynmaicRangeProgram = MGF_RESOURCE.Create<GraphicsProgram>("LowDynamicRangeProgram", Vector<ShaderPtr>{ ldrVs, ldrFs });

		return true;
	}

	void LDREffects::Resize(int32 width, int32 height)
	{

	}

	bool LDREffects::Render(RenderContext* context)
	{
		if (!context || !m_lowDynmaicRangeProgram) return false;
		if (m_lowDynmaicRangeProgram->GetState() != EResourceState::Ready) return false;

		// 1. 핑퐁 버퍼에서 현재 읽기용인 텍스처를 가져옵니다.
		auto sceneColorTex = context->GetSceneColorTexture();
		if (!sceneColorTex) return false;

		// 2. 프로그램 활성화
		m_lowDynmaicRangeProgram->Use();

		// 3. 텍스처 바인딩
		sceneColorTex->Bind(0);
		m_lowDynmaicRangeProgram->SetUniform("uSceneTex", 0);

		// 4. 유니폼 파라미터 전달
		m_lowDynmaicRangeProgram->SetUniform("uToneMappingMode", static_cast<int32>(m_toneMappingMode));
		m_lowDynmaicRangeProgram->SetUniform("uExposure", m_exposure);
		m_lowDynmaicRangeProgram->SetUniform("uGamma", m_gamma);

		m_lowDynmaicRangeProgram->SetUniform("uVignetteIntensity", m_vignetteIntensity);
		m_lowDynmaicRangeProgram->SetUniform("uSharpenAmount", m_sharpenAmount);
		m_lowDynmaicRangeProgram->SetUniform("uGrainAmount", m_grainAmount);

		m_lowDynmaicRangeProgram->SetUniform("uDistortionK", m_distortionK);
		m_lowDynmaicRangeProgram->SetUniform("uScanlineIntensity", m_scanlineIntensity);
		m_lowDynmaicRangeProgram->SetUniform("uPhosphorIntensity", m_phosphorIntensity);
		m_lowDynmaicRangeProgram->SetUniform("uFlickerIntensity", m_flickerIntensity);

		m_lowDynmaicRangeProgram->SetUniform("uTime", (float)MGF_TIME.GetTime());

		// 5. 화면에 그리기
		context->GetScreenMesh()->Draw();

		return true;
	}
}
