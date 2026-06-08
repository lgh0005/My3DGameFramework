#include "RuntimePch.h"
#include "MGFShadowPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Managers/WindowManager.h"
#include "Managers/EntityManager.h"
#include "Assets/Shader.h"
#include "Graphics/Programs/GraphicsProgram.h"
#include "Rendering/RenderContext.h"
#include "Components/Camera.h"
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
		// 0. 공통 셰이더 로드
		auto dirGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Directional.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		auto dirFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Directional.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		auto pointGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Point.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		auto pointFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Point.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		auto spotGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Spot.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		auto spotFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Spot.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);

		// 1. 셰이더 로드
		auto dirVsStatic = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Directional_Static.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto dirVsSkinned = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Directional_Skinned.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		m_dirShadowStaticProgram = MGF_RESOURCE.Create<GraphicsProgram>("DirShadowStaticProgram", Vector<ShaderPtr>{ dirVsStatic, dirGs, dirFs });
		m_dirShadowSkinnedProgram = MGF_RESOURCE.Create<GraphicsProgram>("DirShadowSkinnedProgram", Vector<ShaderPtr>{ dirVsSkinned, dirGs, dirFs });

		auto pointVsStatic = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Point_Static.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto pointVsSkinned = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Point_Skinned.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		m_pointShadowStaticProgram = MGF_RESOURCE.Create<GraphicsProgram>("PointShadowProgram", Vector<ShaderPtr>{ pointVsStatic, pointGs, pointFs });
		m_pointShadowSkinnedProgram = MGF_RESOURCE.Create<GraphicsProgram>("SpotShadowProgram", Vector<ShaderPtr>{ pointVsSkinned, pointGs, pointFs });

		auto spotVsStatic = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Spot_Static.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto spotVsSkinned = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Spot_Skinned.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		m_spotShadowStaticProgram = MGF_RESOURCE.Create<GraphicsProgram>("SpotShadowProgram", Vector<ShaderPtr>{ spotVsStatic, spotGs, spotFs });
		m_spotShadowSkinnedProgram = MGF_RESOURCE.Create<GraphicsProgram>("SpotShadowProgram", Vector<ShaderPtr>{ spotVsSkinned, spotGs, spotFs });

		// 2. FBO 및 텍스처 배열 생성
		// Directional (CSM): 조명당 4개의 레이어 필요
		int32 dirLayers = MAX_LIGHTS * MAX_DIR_SHADOW_LAYERS;
		m_dirShadowMapArray = MGF_RESOURCE.CreateImmediate<GLTexture2DArray>(SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM, dirLayers);
		m_dirShadowFBO = GLFramebuffer2D::CreateArray(m_dirShadowMapArray);

		// Point (Cube): 조명 1개당 6면이므로 CubeArray에서 내부적으로 처리
		m_pointShadowMapArray = MGF_RESOURCE.CreateImmediate<GLTextureCubeArray>(SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM, MAX_POINT_SHADOW_COUNT);
		m_pointShadowFBO = GLFramebufferCube::CreateArray(m_pointShadowMapArray);

		// Spot: 조명 1개당 1개의 레이어
		m_spotShadowMapArray = MGF_RESOURCE.CreateImmediate<GLTexture2DArray>(SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM, MAX_SPOT_SHADOW_LAYERS);
		m_spotShadowFBO = GLFramebuffer2D::CreateArray(m_spotShadowMapArray);

		if (!m_dirShadowFBO || !m_pointShadowFBO || !m_spotShadowFBO) return false;

		return true;
	}

	void MGFShadowPass::Resize()
	{

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
		if (!context) return;

		// Static 프로그램 검증
		if (m_dirShadowStaticProgram->GetState() != EResourceState::Ready   ||
			m_pointShadowStaticProgram->GetState() != EResourceState::Ready ||
			m_spotShadowStaticProgram->GetState() != EResourceState::Ready) return;

		// Skinned 프로그램 검증
		if (m_dirShadowSkinnedProgram->GetState() != EResourceState::Ready   ||
			m_pointShadowSkinnedProgram->GetState() != EResourceState::Ready ||
			m_spotShadowSkinnedProgram->GetState() != EResourceState::Ready) return;

		// 1. 그림자 렌더링 공통 상태 설정
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		// 2. 매쉬 그림자 텍스쳐 베이킹
		RenderDirectionalShadows(context);
		RenderPointShadows(context);
		RenderSpotShadows(context);
	}

	void MGFShadowPass::RenderDirectionalShadows(RenderContext* context)
	{
		const auto& dirLights = context->GetDirectionalLights();
		auto& shadowDataList = context->GetDirectionalShadows();
		if (dirLights.empty() || shadowDataList.empty()) return;

		const Camera* currentCamera = context->GetCurrentCamera();
		if (!currentCamera || !currentCamera->IsMainCamera()) return;

		m_dirShadowFBO->Bind();
		glViewport(0, 0, SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM);
		glClear(GL_DEPTH_BUFFER_BIT);

		m_dirShadowStaticProgram->Use();
		for (usize i = 0; i < dirLights.size(); ++i)
		{
			int32 shadowIdx = dirLights[i].shadowIndex;
			if (shadowIdx < 0) continue;

			DirectionalShadowData& sData = shadowDataList[shadowIdx];
			vec3 lightDir = vec3(dirLights[i].direction);

			// 1. 카메라 시야를 기반으로 CSM 분할 행렬(4단계) 계산
			Vector<mat4> lightMatrices = CalculateCSMMatrices(currentCamera, lightDir, sData.cascadeSplits);
			for (int j = 0; j < 4; ++j) sData.lightSpaceMatrices[j] = lightMatrices[j];

			// 2. Static Queue 처리
			m_dirShadowStaticProgram->Use();
			m_dirShadowStaticProgram->SetUniform("lightSpaceMatrices", lightMatrices);
			m_dirShadowStaticProgram->SetUniform("baseLayerIndex", sData.shadowMapBaseIdx);
			context->GetStaticQueue().Execute(m_dirShadowStaticProgram.get());

			// 3. Skinned Queue 처리
			m_dirShadowSkinnedProgram->Use();
			m_dirShadowSkinnedProgram->SetUniform("lightSpaceMatrices", lightMatrices);
			m_dirShadowSkinnedProgram->SetUniform("baseLayerIndex", sData.shadowMapBaseIdx);
			context->GetSkinnedQueue().Execute(m_dirShadowSkinnedProgram.get());
		}

		// 4. 행렬이 채워진 최종 데이터를 컨텍스트에 갱신하고, 텍스처 캐시에 등록
		context->UpdateDirectionalShadows(shadowDataList);
		context->SetDirectionalShadowMap(m_dirShadowMapArray);
		GLFramebufferHandle::Unbind();
	}

	void MGFShadowPass::RenderPointShadows(RenderContext* context)
	{
		const auto& pointLights = context->GetPointLights();
		auto& shadowDataList = context->GetPointShadows();
		if (pointLights.empty() || shadowDataList.empty()) return;

		m_pointShadowFBO->Bind();
		glViewport(0, 0, SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM);
		glClear(GL_DEPTH_BUFFER_BIT);

		m_pointShadowStaticProgram->Use();

		for (usize i = 0; i < pointLights.size(); ++i)
		{
			if (i >= MAX_POINT_SHADOW_COUNT) break;
			
			int32 shadowIdx = pointLights[i].shadowIndex;
			if (shadowIdx < 0) continue;

			const PointShadowData& sData = shadowDataList[shadowIdx];
			vec3 lightPos = vec3(pointLights[i].position);
			float farPlane = sData.shadowFarPlane;

			mat4 shadowProj = glm::perspective(Math::ToRadians(90.0f), 1.0f, 0.1f, farPlane);
			Vector<mat4> shadowTransforms;
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(1, 0, 0), vec3(0, -1, 0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(-1, 0, 0), vec3(0, -1, 0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 1, 0), vec3(0, 0, 1)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, -1, 0), vec3(0, 0, -1)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 0, 1), vec3(0, -1, 0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 0, -1), vec3(0, -1, 0)));

			// Static Queue 처리
			m_pointShadowStaticProgram->Use();
			m_pointShadowStaticProgram->SetUniform("shadowMatrices", shadowTransforms);
			m_pointShadowStaticProgram->SetUniform("lightPos", lightPos);
			m_pointShadowStaticProgram->SetUniform("farPlane", farPlane);
			m_pointShadowStaticProgram->SetUniform("lightIndex", static_cast<int32>(i));
			context->GetStaticQueue().Execute(m_pointShadowStaticProgram.get());

			// Skinned Queue 처리
			m_pointShadowSkinnedProgram->Use();
			m_pointShadowSkinnedProgram->SetUniform("shadowMatrices", shadowTransforms);
			m_pointShadowSkinnedProgram->SetUniform("lightPos", lightPos);
			m_pointShadowSkinnedProgram->SetUniform("farPlane", farPlane);
			m_pointShadowSkinnedProgram->SetUniform("lightIndex", static_cast<int32>(i));
			context->GetSkinnedQueue().Execute(m_pointShadowSkinnedProgram.get());
		}

		context->SetPointShadowMap(m_pointShadowMapArray);
		GLFramebufferHandle::Unbind();
	}

	void MGFShadowPass::RenderSpotShadows(RenderContext* context)
	{
		const auto& spotLights = context->GetSpotLights();
		const auto& shadowDataList = context->GetSpotShadows();
		if (spotLights.empty() || shadowDataList.empty()) return;

		m_spotShadowFBO->Bind();
		glViewport(0, 0, SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM);
		glClear(GL_DEPTH_BUFFER_BIT);

		m_spotShadowStaticProgram->Use();

		for (usize i = 0; i < spotLights.size(); ++i)
		{
			int32 shadowIdx = spotLights[i].shadowIndex;
			if (shadowIdx < 0) continue;

			// Spot Light는 Collector에서 이미 View-Proj 행렬(lightSpaceMatrix)을 계산해 두었습니다.
			const SpotShadowData& sData = shadowDataList[shadowIdx];

			// Static Queue 처리
			m_spotShadowStaticProgram->Use();
			m_spotShadowStaticProgram->SetUniform("lightSpaceMatrix", sData.lightSpaceMatrix);
			m_spotShadowStaticProgram->SetUniform("layerIndex", sData.shadowMapIdx);
			context->GetStaticQueue().Execute(m_spotShadowStaticProgram.get());

			// Skinned Queue 처리
			m_spotShadowSkinnedProgram->Use();
			m_spotShadowSkinnedProgram->SetUniform("lightSpaceMatrix", sData.lightSpaceMatrix);
			m_spotShadowSkinnedProgram->SetUniform("layerIndex", sData.shadowMapIdx);
			context->GetSkinnedQueue().Execute(m_spotShadowSkinnedProgram.get());
		}

		context->SetSpotShadowMap(m_spotShadowMapArray);
		GLFramebufferHandle::Unbind();
	}

	Vector<mat4> MGFShadowPass::CalculateCSMMatrices
	(
		const Camera* camera,
		const vec3& lightDir,
		vec4& outSplits
	)
	{
		Vector<mat4> ret;
		if (!camera) return ret;

		float nearPlane = camera->GetNear();
		float farPlane = camera->GetFar();
		float fov = camera->GetFOV();
		float aspect = camera->GetAspectRatio();

		mat4 camView = camera->GetViewMatrix();
		mat4 camInvView = Math::Inverse(camView);

		float cascadeLevels[4];
		cascadeLevels[0] = nearPlane + (farPlane - nearPlane) * 0.05f;
		cascadeLevels[1] = nearPlane + (farPlane - nearPlane) * 0.15f;
		cascadeLevels[2] = nearPlane + (farPlane - nearPlane) * 0.50f;
		cascadeLevels[3] = farPlane;

		outSplits = vec4(cascadeLevels[0], cascadeLevels[1], cascadeLevels[2], cascadeLevels[3]);

		float lastSplitDist = nearPlane;

		for (int i = 0; i < 4; ++i)
		{
			float currentSplitDist = cascadeLevels[i];

			mat4 proj = glm::perspective(Math::ToRadians(fov), aspect, lastSplitDist, currentSplitDist);
			mat4 invCam = camInvView * Math::Inverse(proj);

			vec3 corners[8] = {
				vec3(-1.0f, -1.0f, -1.0f), vec3(1.0f, -1.0f, -1.0f),
				vec3(1.0f,  1.0f, -1.0f), vec3(-1.0f,  1.0f, -1.0f),
				vec3(-1.0f, -1.0f,  1.0f), vec3(1.0f, -1.0f,  1.0f),
				vec3(1.0f,  1.0f,  1.0f), vec3(-1.0f,  1.0f,  1.0f)
			};

			vec3 center = vec3(0.0f);
			for (int j = 0; j < 8; ++j)
			{
				vec4 pt = invCam * vec4(corners[j], 1.0f);
				corners[j] = vec3(pt) / pt.w;
				center += corners[j];
			}
			center /= 8.0f;

			vec3 lightDirNorm = glm::normalize(lightDir);
			vec3 tempUp = (glm::abs(lightDirNorm.z) > 0.999f) ? vec3(0.0f, 1.0f, 0.0f) : vec3(0.0f, 0.0f, 1.0f);
			vec3 right = glm::normalize(glm::cross(tempUp, lightDirNorm));
			vec3 up = glm::normalize(glm::cross(lightDirNorm, right));
			mat4 lightView = glm::lookAt(center - lightDirNorm, center, up);

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::lowest();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::lowest();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::lowest();

			for (int j = 0; j < 8; ++j)
			{
				vec4 trf = lightView * vec4(corners[j], 1.0f);
				minX = Math::Min(minX, trf.x);
				maxX = Math::Max(maxX, trf.x);
				minY = Math::Min(minY, trf.y);
				maxY = Math::Max(maxY, trf.y);
				minZ = Math::Min(minZ, trf.z);
				maxZ = Math::Max(maxZ, trf.z);
			}

			float zNear = -maxZ;
			float zFar = -minZ;

			constexpr float shadowCasterBackDistance = 150.0f;
			zNear -= shadowCasterBackDistance;
			zFar += shadowCasterBackDistance;

			mat4 lightProj = glm::ortho(minX, maxX, minY, maxY, zNear, zFar);

			ret.push_back(lightProj * lightView);
			lastSplitDist = currentSplitDist;
		}

		return ret;
	}
}