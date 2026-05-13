#include "RuntimePch.h"
#include "MGFShadowPass.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
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
		// 1. 셰이더 로드
		auto dirVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Directional.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto dirGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Directional.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		auto dirFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Directional.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_dirShadowProgram = MGF_RESOURCE.Create<GraphicsProgram>("DirShadowProgram", Vector<ShaderPtr>{ dirVs, dirGs, dirFs });

		auto pointVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Point.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto pointGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Point.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		auto pointFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Point.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_pointShadowProgram = MGF_RESOURCE.Create<GraphicsProgram>("PointShadowProgram", Vector<ShaderPtr>{ pointVs, pointGs, pointFs });

		auto spotVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Spot.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto spotGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Spot.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);
		auto spotFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/Default/MGF3D_Shadow_Spot.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_spotShadowProgram = MGF_RESOURCE.Create<GraphicsProgram>("SpotShadowProgram", Vector<ShaderPtr>{ spotVs, spotGs, spotFs });
		
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
		if (m_dirShadowProgram->GetState() != EResourceState::Ready ||
			m_pointShadowProgram->GetState() != EResourceState::Ready ||
			m_spotShadowProgram->GetState() != EResourceState::Ready) return;

		// 1. 그림자 렌더링 공통 상태 설정
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		// 2. 그림자 텍스쳐 베이킹
		RenderDirectionalShadows(context);
		RenderPointShadows(context);
		RenderSpotShadows(context);
	}

	void MGFShadowPass::RenderDirectionalShadows(RenderContext* context)
	{
		const auto& dirLights = context->GetDirectionalLights();
		Vector<DirectionalShadowData> shadowDataList = context->GetDirectionalShadows();
		if (dirLights.empty() || shadowDataList.empty()) return;

		const Camera* currentCamera = context->GetCurrentCamera();
		if (!currentCamera || !currentCamera->IsMainCamera()) return;

		m_dirShadowFBO->Bind();
		glViewport(0, 0, SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM);
		glClear(GL_DEPTH_BUFFER_BIT);

		m_dirShadowProgram->Use();
		for (usize i = 0; i < dirLights.size(); ++i)
		{
			int32 shadowIdx = dirLights[i].shadowIndex;
			if (shadowIdx < 0) continue;

			DirectionalShadowData& sData = shadowDataList[shadowIdx];
			vec3 lightDir = vec3(dirLights[i].direction);

			// 1. 카메라 시야를 기반으로 CSM 분할 행렬(4단계) 계산
			Vector<mat4> lightMatrices = CalculateCSMMatrices(currentCamera, lightDir, sData.cascadeSplits);
			for (int j = 0; j < 4; ++j) sData.lightSpaceMatrices[j] = lightMatrices[j];

			// 2. 셰이더로 행렬 배열 및 대상 Layer(층) 인덱스 전달
			m_dirShadowProgram->SetUniform("lightSpaceMatrices", lightMatrices);
			m_dirShadowProgram->SetUniform("baseLayerIndex", sData.shadowMapBaseIdx);

			// 3. RenderQueue에 쌓인 메쉬들을 그림자 셰이더를 덮어씌워 렌더링
			context->GetStaticQueue().Execute(m_dirShadowProgram.get());
			context->GetSkinnedQueue().Execute(m_dirShadowProgram.get());
		}

		// 4. 행렬이 채워진 최종 데이터를 컨텍스트에 갱신하고, 텍스처 캐시에 등록
		context->UpdateDirectionalShadows(shadowDataList);
		context->SetDirectionalShadowMap(m_dirShadowMapArray);
		GLFramebufferHandle::Unbind();
	}

	void MGFShadowPass::RenderPointShadows(RenderContext* context)
	{
		auto& pointLights = context->GetPointLights();
		if (pointLights.empty()) return;

		m_pointShadowFBO->Bind();
		glViewport(0, 0, SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM);
		glClear(GL_DEPTH_BUFFER_BIT);

		m_pointShadowProgram->Use();

		Vector<PointShadowData> shadowDataList;

		for (usize i = 0; i < pointLights.size(); ++i)
		{
			if (i >= MAX_POINT_SHADOW_COUNT) break;
			
			pointLights[i].shadowIndex = static_cast<int32>(i);
			vec3 lightPos = vec3(pointLights[i].position);
			float farPlane = pointLights[i].position.w;

			mat4 shadowProj = glm::perspective(Math::ToRadians(90.0f), 1.0f, 0.1f, farPlane);
			Vector<mat4> shadowTransforms;
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(1, 0, 0), vec3(0, -1, 0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(-1, 0, 0), vec3(0, -1, 0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 1, 0), vec3(0, 0, 1)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, -1, 0), vec3(0, 0, -1)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 0, 1), vec3(0, -1, 0)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + vec3(0, 0, -1), vec3(0, -1, 0)));

			m_pointShadowProgram->SetUniform("shadowMatrices", shadowTransforms);
			m_pointShadowProgram->SetUniform("lightPos", lightPos);
			m_pointShadowProgram->SetUniform("farPlane", farPlane);
			m_pointShadowProgram->SetUniform("lightIndex", static_cast<int32>(i));

			context->GetStaticQueue().Execute(m_pointShadowProgram.get());
			context->GetSkinnedQueue().Execute(m_pointShadowProgram.get());

			PointShadowData sData;
			sData.shadowFarPlane = farPlane;
			sData.shadowMapIdx = static_cast<int32>(i);
			sData.shadowBias = 0.05f;
			shadowDataList.push_back(sData);
		}

		context->UpdatePointShadows(shadowDataList);
		context->SetPointShadowMap(m_pointShadowMapArray);
		context->UpdatePointLights(pointLights);
		GLFramebufferHandle::Unbind();
	}

	void MGFShadowPass::RenderSpotShadows(RenderContext* context)
	{
		const auto& spotLights = context->GetSpotLights();
		if (spotLights.empty()) return;

		m_spotShadowFBO->Bind();
		glViewport(0, 0, SHADOW_RES_MEDIUM, SHADOW_RES_MEDIUM);
		glClear(GL_DEPTH_BUFFER_BIT);

		m_spotShadowProgram->Use();

		Vector<SpotShadowData> shadowDataList = context->GetSpotShadows();

		for (size_t i = 0; i < spotLights.size(); ++i)
		{
			int shadowIdx = spotLights[i].shadowIndex;
			if (shadowIdx < 0) continue;

			// Spot Light는 Collector에서 이미 View-Proj 행렬(lightSpaceMatrix)을 계산해 두었습니다.
			const SpotShadowData& sData = shadowDataList[shadowIdx];

			m_spotShadowProgram->SetUniform("lightSpaceMatrix", sData.lightSpaceMatrix);
			m_spotShadowProgram->SetUniform("layerIndex", sData.shadowMapIdx);

			context->GetStaticQueue().Execute(m_spotShadowProgram.get());
			context->GetSkinnedQueue().Execute(m_spotShadowProgram.get());
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

			vec3 up = (Math::Abs(lightDir.y) > 0.999f) ? vec3(0.0f, 0.0f, 1.0f) : vec3(0.0f, 1.0f, 0.0f);
			mat4 lightView = glm::lookAt(center - lightDir, center, up);

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

			constexpr float zMultiplier = 10.0f;
			if (minZ < 0) minZ *= zMultiplier; else minZ /= zMultiplier;
			if (maxZ < 0) maxZ /= zMultiplier; else maxZ *= zMultiplier;

			mat4 lightProj = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

			ret.push_back(lightProj * lightView);
			lastSplitDist = currentSplitDist;
		}

		return ret;
	}
}