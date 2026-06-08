#include "RuntimePch.h"
#include "WorldSpaceEffects.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h""
#include "Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"
#include "Textures/GLTexture2D.h"
#include "Meshes/ScreenMesh.h"
#include "Components/Camera.h"
#include "Components/Transform.h"

namespace MGF3D
{
	WorldSpaceEffects::WorldSpaceEffects() = default;
	WorldSpaceEffects::~WorldSpaceEffects() = default;

	WorldSpaceEffectsUPtr WorldSpaceEffects::Create()
	{
		auto effect = WorldSpaceEffectsUPtr(new WorldSpaceEffects());
		if (!effect->Init()) return nullptr;
		return effect;
	}

	bool WorldSpaceEffects::Init()
	{
		// 1. HDR 효과용 프로그램 로드
		auto worldVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/PostProcessing/MGF3D_PostProcess_World.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto worldFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/PostProcessing/MGF3D_PostProcess_World.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_worldSpaceEffectProgram = MGF_RESOURCE.Create<GraphicsProgram>("WorldSpaceEffectProgram", Vector<ShaderPtr>{ worldVs, worldFs });

		return true;
	}

	void WorldSpaceEffects::Resize(int32 width, int32 height)
	{

	}

	bool WorldSpaceEffects::Render(RenderContext* context)
	{
		if (!context || !m_worldSpaceEffectProgram) return false;
		if (m_worldSpaceEffectProgram->GetState() != EResourceState::Ready) return false;

		// 1. 텍스처 가져오기 (핑퐁 버퍼 중 현재 읽기용인 A버퍼 컬러와 G-Buffer 위치 정보)
		auto sceneColorTex = context->GetSceneColorTexture();
		auto positionTex = context->GetGeometryBufferTexture(EGBufferSlot::PositionAO);
		if (!sceneColorTex || !positionTex) return false;

		// 2. 프로그램 바인딩
		m_worldSpaceEffectProgram->Use();

		// 3. 텍스처 바인딩 (슬롯 지정)
		sceneColorTex->Bind(0);
		positionTex->Bind(1);

		// 4. 유니폼 값 설정 (GraphicsProgram의 SetUniform 사용)
		m_worldSpaceEffectProgram->SetUniform("uSceneTex", 0);
		m_worldSpaceEffectProgram->SetUniform("uPositionTex", 1);

		m_worldSpaceEffectProgram->SetUniform("uPixelSize", m_pixelSize);
		m_worldSpaceEffectProgram->SetUniform("uDilationRadius", m_dilationRadius);
		m_worldSpaceEffectProgram->SetUniform("uPostLevels", m_postLevels);
		m_worldSpaceEffectProgram->SetUniform("uFogColor", m_fogColor);
		m_worldSpaceEffectProgram->SetUniform("uFogDensity", m_fogDensity);

		// 카메라 위치 넘겨주기 (안개 거리 계산용)
		const Camera* camera = context->GetCurrentCamera();
		if (camera)
		{
			auto transform = camera->GetTransform();
			m_worldSpaceEffectProgram->SetUniform("uCameraPos", transform->GetWorldPosition());
		}

		// 5. 화면(B버퍼)에 그리기
		context->GetScreenMesh()->Draw();

		return true;
	}
}