#include "RuntimePch.h"
#include "MGFShadowPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"
#include "TextureArrays/GLTexture2DArray.h"
#include "TextureArrays/GLTextureCubeArray.h"
#include "Framebuffers/GLFramebuffer2D.h"
#include "Framebuffers/GLFramebufferCube.h"
#include "Instancing/Shadows/DirectionalShadowData.h"
#include "Instancing/Shadows/PointShadowData.h"
#include "Instancing/Shadows/SpotShadowData.h"

namespace MGF3D
{
	MGFShadowPass::MGFShadowPass() = default;
	MGFShadowPass::~MGFShadowPass() = default;

	MGFShadowPassUPtr MGFShadowPass::Create()
	{
		auto renderPass = MGFShadowPassUPtr(new MGFShadowPass());
		if (!renderPass->Init()) return nullptr;
		return renderPass;
	}

	bool MGFShadowPass::Init()
	{
		//// 1. 셰이더 로드 (Vertex, Geometry, Fragment)
		//auto dirVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/DirShadow.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		//auto dirGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/DirShadow.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		//auto dirFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/DirShadow.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		//m_dirShadowProgram = MGF_RESOURCE.Create<GraphicsProgram>("DirShadowProgram", Vector<ShaderPtr>{ dirVs, dirGs, dirFs });

		//auto pointVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/PointShadow.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		//auto pointGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/PointShadow.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		//auto pointFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/PointShadow.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		//m_pointShadowProgram = MGF_RESOURCE.Create<GraphicsProgram>("PointShadowProgram", Vector<ShaderPtr>{ pointVs, pointGs, pointFs });

		//auto spotVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/SpotShadow.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		//auto spotGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/SpotShadow.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		//auto spotFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Shadow/SpotShadow.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		//m_spotShadowProgram = MGF_RESOURCE.Create<GraphicsProgram>("SpotShadowProgram", Vector<ShaderPtr>{ spotVs, spotGs, spotFs });
		//
		//// 2. FBO 및 텍스처 배열 생성
		//// Directional (CSM): 조명당 4개의 레이어 필요
		//int32 dirLayers = MAX_LIGHTS * MAX_DIR_SHADOW_LAYERS;
		//m_dirShadowMapArray = MGF_RESOURCE.CreateImmediate<GLTexture2DArray>(SHADOW_RES_HIGH, SHADOW_RES_HIGH, dirLayers);
		//m_dirShadowFBO = GLFramebuffer2D::CreateArray(m_dirShadowMapArray);

		//// Point (Cube): 조명 1개당 6면이므로 CubeArray에서 내부적으로 처리
		//m_pointShadowMapArray = GLTextureCubeArray::Create(SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM, MAX_POINT_SHADOW_COUNT);
		//m_pointShadowFBO = GLFramebufferCube::CreateArray(m_pointShadowMapArray);

		//// Spot: 조명 1개당 1개의 레이어
		//m_spotShadowMapArray = MGF_RESOURCE.CreateImmediate<GLTexture2DArray>(SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM, MAX_SPOT_SHADOW_LAYERS);
		//m_spotShadowFBO = GLFramebuffer2D::CreateArray(m_spotShadowMapArray);

		//if (!m_dirShadowFBO || !m_pointShadowFBO || !m_spotShadowFBO) return false;

		return true;
	}

	/*==============================//
	//    RenderPass Custom Type    //
	//==============================*/
	int16 MGFShadowPass::s_typeIndex = -1;
	const MGFType* MGFShadowPass::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("RenderPass");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void MGFShadowPass::Execute(RenderContext* context)
	{
		//if (!context) return;
		//if (m_dirShadowProgram->GetState() != EResourceState::Ready ||
		//	m_pointShadowProgram->GetState() != EResourceState::Ready ||
		//	m_spotShadowProgram->GetState() != EResourceState::Ready) return;

		//// 1. 그림자 렌더링 공통 상태 설정
		//glEnable(GL_DEPTH_TEST);
		//glCullFace(GL_FRONT);

		//// 2. 그림자 텍스쳐 베이킹
		//RenderDirectionalShadows(context);
		//RenderPointShadows(context);
		//RenderSpotShadows(context);

		//// 3. 원상태로 복구
		//glCullFace(GL_BACK);
	}

	void MGFShadowPass::RenderDirectionalShadows(RenderContext* context)
	{
		//auto& dirLights = context->GetDirectionalLights();
		//if (dirLights.empty()) return;

		//m_dirShadowFBO->Bind();
		//glViewport(0, 0, SHADOW_RES_HIGH, SHADOW_RES_HIGH);
		//glClear(GL_DEPTH_BUFFER_BIT);

		//m_dirShadowProgram->Use();

		//Vector<DirectionalShadowData> shadowDataList;
		//int32 currentBaseIdx = 0;

		//for (size_t i = 0; i < dirLights.size(); ++i)
		//{
		//	if (i >= MAX_LIGHTS) break;

		//	DirectionalShadowData sData;
		//	sData.shadowMapBaseIdx = currentBaseIdx;
		//	sData.shadowBias = 0.005f;

		//	// CSM 분할 행렬 계산 (추후 구현)
		//	// Vector<mat4> lightMatrices = CalculateCSMMatrices(context->GetCameraView(), context->GetCameraProj(), dirLights[i].direction, sData.cascadeSplits);
		//	// for (int j = 0; j < 4; ++j) sData.lightSpaceMatrices[j] = lightMatrices[j];

		//	// m_dirShadowProgram->SetUniformArray("lightSpaceMatrices", lightMatrices.data(), 4);
		//	m_dirShadowProgram->SetUniform("baseLayerIndex", currentBaseIdx);

		//	context->GetStaticQueue().Execute(m_dirShadowProgram.get());
		//	context->GetSkinnedQueue().Execute(m_dirShadowProgram.get());

		//	shadowDataList.push_back(sData);
		//	currentBaseIdx += MAX_DIR_SHADOW_LAYERS;
		//}

		//context->UpdateDirectionalShadows(shadowDataList);
		//context->SetCachedTexture(ETextureCache::DirShadowArray, m_dirShadowMapArray);
		//GLFramebufferHandle::Unbind();
	}

	void MGFShadowPass::RenderPointShadows(RenderContext* context)
	{
		//auto& pointLights = context->GetPointLights();
		//if (pointLights.empty()) return;

		//m_pointShadowFBO->Bind();
		//glViewport(0, 0, SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM);
		//glClear(GL_DEPTH_BUFFER_BIT);

		//m_pointShadowProgram->Use();

		//Vector<PointShadowData> shadowDataList;

		//for (size_t i = 0; i < pointLights.size(); ++i)
		//{
		//	if (i >= MAX_POINT_SHADOW_COUNT) break;

		//	vec3 lightPos = vec3(pointLights[i].position);
		//	float farPlane = pointLights[i].position.w;

		//	mat4 shadowProj = glm::perspective(Math::ToRadians(90.0f), 1.0f, 0.1f, farPlane);
		//	Vector<mat4> shadowTransforms;
		//	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(1, 0, 0), vec3(0, -1, 0)));
		//	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(-1, 0, 0), vec3(0, -1, 0)));
		//	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 1, 0), vec3(0, 0, 1)));
		//	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, -1, 0), vec3(0, 0, -1)));
		//	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 0, 1), vec3(0, -1, 0)));
		//	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 0, -1), vec3(0, -1, 0)));

		//	m_pointShadowProgram->SetUniformArray("shadowMatrices", shadowTransforms.data(), 6);
		//	m_pointShadowProgram->SetUniform("lightPos", lightPos);
		//	m_pointShadowProgram->SetUniform("farPlane", farPlane);
		//	m_pointShadowProgram->SetUniform("lightIndex", static_cast<int>(i));

		//	context->GetStaticQueue().Execute(m_pointShadowProgram.get());
		//	context->GetSkinnedQueue().Execute(m_pointShadowProgram.get());

		//	PointShadowData sData;
		//	sData.shadowFarPlane = farPlane;
		//	sData.shadowMapIdx = static_cast<int32>(i);
		//	sData.shadowBias = 0.05f;
		//	shadowDataList.push_back(sData);
		//}

		//context->UpdatePointShadows(shadowDataList);
		//context->SetCachedTexture(ETextureCache::PointShadowArray, m_pointShadowMapArray);
		//GLFramebufferHandle::Unbind();
	}

	void MGFShadowPass::RenderSpotShadows(RenderContext* context)
	{
		//auto& spotLights = context->GetSpotLights();
		//if (spotLights.empty()) return;

		//m_spotShadowFBO->Bind();
		//glViewport(0, 0, SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM);
		//glClear(GL_DEPTH_BUFFER_BIT);

		//m_spotShadowProgram->Use();

		//Vector<SpotShadowData> shadowDataList;

		//for (size_t i = 0; i < spotLights.size(); ++i)
		//{
		//	if (i >= MAX_SPOT_SHADOW_LAYERS) break;

		//	const auto& light = spotLights[i];
		//	vec3 lightPos = vec3(light.position);
		//	vec3 lightDir = vec3(light.direction);

		//	float fov = Math::ACos(light.cutOff[1]) * 2.0f;
		//	float farPlane = light.position.w;

		//	mat4 lightProj = glm::perspective(fov, 1.0f, 0.1f, farPlane);
		//	mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, vec3(0.0f, 1.0f, 0.0f));
		//	mat4 lightSpaceMatrix = lightProj * lightView;

		//	m_spotShadowProgram->SetUniform("lightSpaceMatrix", lightSpaceMatrix);
		//	m_spotShadowProgram->SetUniform("layerIndex", static_cast<int>(i));

		//	context->GetStaticQueue().Execute(m_spotShadowProgram.get());
		//	context->GetSkinnedQueue().Execute(m_spotShadowProgram.get());

		//	SpotShadowData sData;
		//	sData.lightSpaceMatrix = lightSpaceMatrix;
		//	sData.shadowMapIdx = static_cast<int32>(i);
		//	sData.shadowBias = 0.005f;
		//	shadowDataList.push_back(sData);
		//}

		//context->UpdateSpotShadows(shadowDataList);
		//context->SetCachedTexture(ETextureCache::SpotShadowArray, m_spotShadowMapArray);

		//GLFramebufferHandle::Unbind();
	}

	Vector<mat4> MGFShadowPass::CalculateCSMMatrices
	(
		const mat4& cameraView, 
		const mat4& cameraProj, 
		const vec3& lightDir, 
		vec4& outSplits
	)
	{
		//// TODO: CSM 행렬 분할 수학 로직 구현
		return Vector<mat4>(4, mat4(1.0f));
	}
}