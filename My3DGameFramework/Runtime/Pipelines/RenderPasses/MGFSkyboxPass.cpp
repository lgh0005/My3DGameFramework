#include "RuntimePch.h"
#include "MGFSkyboxPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"
#include "Components/Camera.h"
#include "Components/Lights/SkyLight.h"
#include "Resources/EnvironmentMap.h"
#include "Buffers/GLUniformBuffer.h"
#include "Meshes/StaticMesh.h"
#include "GraphicsUtils/GeometryUtils.h"
#include "Graphics/Framebuffers/GLFramebuffer2D.h"
#include "Uniforms/SkyboxUniform.h"
#include "Textures/GLTextureCube.h"

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
		// 1. 스카이박스 전용 셰이더 로드
		auto vs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Skybox.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto fs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Skybox.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_skyboxProgram = MGF_RESOURCE.Create<GraphicsProgram>("SkyboxProgram", Vector<ShaderPtr>{ vs, fs });
		
		// 2. UBO 생성
		m_skyboxUBO = GLUniformBuffer::Create(nullptr, sizeof(SkyboxData));

		// 3. 큐브 메쉬 생성
		m_cubeMesh = GeometryUtils::CreateBox();

		return (m_skyboxProgram && m_skyboxUBO && m_cubeMesh);
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
		if (!context) return;

		// 1. 카메라 및 하늘 데이터 가져오기
		auto sceneBuffer = context->GetSceneBuffer();
		const Camera* camera = context->GetCurrentCamera();
		const SkyLight* skyLight = context->GetMainSkyLight();
		if (!camera || !skyLight) return;
		sceneBuffer->Bind();

		// 2. 텍스처 상태 검증
		EnvironmentMapPtr envMap = skyLight->GetEnvironmentMap();
		if (!envMap || envMap->GetState() != EResourceState::Ready) return;

		GLTextureCubePtr skyboxTex = envMap->GetSkybox();
		if (!skyboxTex) return;

		// 3. UBO 데이터 갱신
		SkyboxData uboData;

		// 뷰 행렬에서 이동(Translation) 성분 제거. 하늘은 무한히 멀리 있어야 합니다.
		uboData.view = mat4(mat3(camera->GetViewMatrix()));
		uboData.projection = camera->GetProjectionMatrix();
		uboData.tint = vec4(skyLight->GetTint(), 1.0f);
		uboData.intensity = skyLight->GetIntensity();
		uboData.rotation = skyLight->GetRotation();

		// 셰이더의 Uniform Block binding index(예: 2)와 맞춰서 바인딩합니다.
		m_skyboxUBO->UpdateData(&uboData, 0, sizeof(SkyboxData));
		m_skyboxUBO->Bind(2);

		// 4. OpenGL 렌더링 상태 설정
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		m_skyboxProgram->Use();
		skyboxTex->Bind(0);

		// 5. 큐브 그리기
		m_cubeMesh->Bind();
		glDrawElements(GL_TRIANGLES, m_cubeMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

		// 6. 상태 복구
		glEnable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
	}
}