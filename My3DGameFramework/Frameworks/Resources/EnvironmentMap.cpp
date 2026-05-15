#include "FrameworkPch.h"
#include "EnvironmentMap.h"
#include "Managers/TypeManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AssetManager.h"
#include "Textures/GLTexture2D.h"
#include "Textures/GLTextureCube.h"
#include "GraphicsUtils/GeometryUtils.h"
#include "Programs/GraphicsProgram.h"
#include "Meshes/StaticMesh.h"
#include "Layout/GLVertexLayout.h"
#include "Buffers/GLUniformBuffer.h"
#include "Framebuffers/GLFramebufferCube.h"
#include "Assets/Shader.h"
#include "Assets/Image.h"
#include "Uniforms/IBLUniform.h"

namespace MGF3D
{
	EnvironmentMap::EnvironmentMap(StringView name) : Super(name) { }
	EnvironmentMap::~EnvironmentMap() = default;

	/*========================//
	//     Material Type      //
	//========================*/
	int16 EnvironmentMap::s_typeIndex = -1;
	const MGFType* EnvironmentMap::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	EnvironmentMapPtr EnvironmentMap::Create(StringView mapName)
	{
		auto envMap = EnvironmentMapPtr(new EnvironmentMap(mapName));
		if (!envMap->Init(mapName)) return nullptr;
		envMap->SetState(EResourceState::Loaded);
		return envMap;
	}

	bool EnvironmentMap::Init(StringView mapName)
	{
		m_name = mapName;

		// 1. 공통 셰이더 (여러 프로그램에서 공유)
		auto commonVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/IBL/MGF3D_IBL_Common.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto commonGs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/IBL/MGF3D_IBL_Layered.geom", GL_GEOMETRY_SHADER, EShaderFileType::GLSL);

		// 2. Skybox (Spherical to Cubemap)
		auto skyboxFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/IBL/MGF3D_IBL_Spherical_Map.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_skyboxProgram = MGF_RESOURCE.Create<GraphicsProgram>("Prog_IBL_Skybox", Vector<ShaderPtr>{ commonVs, commonGs, skyboxFs });

		// 3. Irradiance (Convolution)
		auto irradianceFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/IBL/MGF3D_IBL_Diffuse_Irradiance.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_irradianceProgram = MGF_RESOURCE.Create<GraphicsProgram>("Prog_IBL_Irradiance", Vector<ShaderPtr>{ commonVs, commonGs, irradianceFs });

		// 4. Prefilter (Specular Prefiltering)
		auto prefilterFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/IBL/MGF3D_IBL_Prefiltered_Light.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_prefilterProgram = MGF_RESOURCE.Create<GraphicsProgram>("Prog_IBL_Prefilter", Vector<ShaderPtr>{ commonVs, commonGs, prefilterFs });

		// 5. BRDF LUT - 2D 평면 렌더링이므로 고유 VS, FS 사용
		auto brdfVs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/IBL/MGF3D_IBL_BRDF_Lookup.vert", GL_VERTEX_SHADER, EShaderFileType::GLSL);
		auto brdfFs = MGF_ASSET.LoadAssetAsync<Shader>("@BuiltInAsset/Shaders/IBL/MGF3D_IBL_BRDF_Lookup.frag", GL_FRAGMENT_SHADER, EShaderFileType::GLSL);
		m_brdfProgram = MGF_RESOURCE.Create<GraphicsProgram>("Prog_IBL_BRDF", Vector<ShaderPtr>{ brdfVs, brdfFs });

		return (m_skyboxProgram && m_irradianceProgram && m_prefilterProgram && m_brdfProgram);
	}

	bool EnvironmentMap::OnSyncCreate()
	{
		// 0. HDR 이미지 텍스처 준비 여부 확인
		if (!m_environmentCubeImage) return false;

		EAssetState imgState = m_environmentCubeImage->GetState();
		if (imgState == EAssetState::Failed)
		{
			MGF_LOG_ERROR("EnvironmentMap: HDR image failed to load.");
			SetState(EResourceState::Failed);
			return false;
		}
		if (imgState != EAssetState::Ready)
			return false;

		// 1. 셰이더 프로그램 준비 여부 확인 (이것도 비동기 로딩)
		if (!m_skyboxProgram || m_skyboxProgram->GetState() != EResourceState::Ready ||
			!m_irradianceProgram || m_irradianceProgram->GetState() != EResourceState::Ready ||
			!m_prefilterProgram || m_prefilterProgram->GetState() != EResourceState::Ready ||
			!m_brdfProgram || m_brdfProgram->GetState() != EResourceState::Ready) return false;

		// 2. 임시 도구 생성 (FBO는 원시 API 직접 호출)
		uint32 tempFBO = 0;
		glCreateFramebuffers(1, &tempFBO);

		// 3. IBL 통합 데이터 구성
		IBLData iblData;
		mat4 proj = glm::perspective(Math::ToRadians(90.0f), 1.0f, 0.1f, 10.0f);
		mat4 views[6] = {
			lookAt(vec3(0.0f), vec3(1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f), vec3(0.0f,  1.0f,  0.0f), vec3(0.0f,  0.0f,  1.0f)),
			lookAt(vec3(0.0f), vec3(0.0f, -1.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f)),
			lookAt(vec3(0.0f), vec3(0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f)),
			lookAt(vec3(0.0f), vec3(0.0f,  0.0f, -1.0f), vec3(0.0f, -1.0f,  0.0f))
		};
		for (int i = 0; i < 6; ++i) iblData.VPs[i] = proj * views[i];
		iblData.roughness = 0.0f;

		// UBO는 래퍼 클래스 활용
		auto bakeUBO = GLUniformBuffer::Create(&iblData, sizeof(IBLData));

		auto cubeMesh = GeometryUtils::CreateBox();
		auto screenMesh = GeometryUtils::CreatePlane();
		if (!cubeMesh || !screenMesh)
		{
			glDeleteFramebuffers(1, &tempFBO);
			return false;
		}
		cubeMesh->Bind();
		screenMesh->Bind();

		// 3. 리소스 바인딩
		bakeUBO->Bind(1);
		uint32 cubeVAO = cubeMesh->GetVertexLayout()->GetHandle();
		uint32 screenVAO = screenMesh->GetVertexLayout()->GetHandle();
		uint32 cubeIndexCount = cubeMesh->GetIndexCount();

		// 4. 베이킹 단계 수행
		BakeSkybox(tempFBO, cubeVAO, cubeIndexCount);
		BakeIrradiance(tempFBO, cubeVAO, cubeIndexCount);
		BakePrefiltered(tempFBO, bakeUBO, cubeVAO, cubeIndexCount);
		BakeBRDF(tempFBO, screenVAO, cubeIndexCount);

		// 5. 임시 자원 파괴
		glDeleteFramebuffers(1, &tempFBO);

		m_state = EResourceState::Ready;
		return true;
	}

	void EnvironmentMap::SetCubeTexture(const ImagePtr& image)
	{
		m_environmentCubeImage = image;
	}

	void EnvironmentMap::BakeSkybox(uint32 fbo, uint32 cubeVAO, usize indexCount)
	{
		const int32 res = 1024;
		m_skybox = MGF_RESOURCE.CreateImmediate<GLTextureCube>(res, GL_RGB16F, 0);

		// HDR 소스 이미지로부터 임시 2D 텍스처 생성
		const auto& resources = m_environmentCubeImage->GetResources();
		if (resources.empty()) return;

		auto hdrTex = MGFTypeCaster::Cast< GLTexture2D>(resources[0]);
		if (!hdrTex) return;

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_skybox->GetHandle(), 0);
		glViewport(0, 0, res, res);

		m_skyboxProgram->Use();
		hdrTex->Bind(0);

		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

		m_skybox->GenerateMipmap();
	}

	void EnvironmentMap::BakeIrradiance(uint32 fbo, uint32 cubeVAO, usize indexCount)
	{
		const int32 res = 32;
		m_irradiance = MGF_RESOURCE.CreateImmediate<GLTextureCube>(res, GL_RGB16F, 1);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, m_irradiance->GetHandle(), 0); // DSA 적용
		glViewport(0, 0, res, res);

		m_irradianceProgram->Use();
		m_skybox->Bind(0);

		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void EnvironmentMap::BakePrefiltered(uint32 fbo, const GLUniformBufferUPtr& ubo, uint32 cubeVAO, usize indexCount)
	{
		const int32 baseRes = 512;
		m_prefiltered = MGF_RESOURCE.CreateImmediate<GLTextureCube>(baseRes, GL_RGB16F, 0);
		m_prefiltered->GenerateMipmap();

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		m_prefilterProgram->Use();
		m_skybox->Bind(0);

		const uint32 maxMipLevels = 5;
		usize roughnessOffset = sizeof(mat4) * 6;

		for (uint32 mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32 mipRes = (uint32)(baseRes * std::pow(0.5, mip));
			glViewport(0, 0, mipRes, mipRes);

			// 거칠기 계산 및 UBO 갱신
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			ubo->UpdateData(&roughness, roughnessOffset, sizeof(float));

			glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, m_prefiltered->GetHandle(), mip);

			glBindVertexArray(cubeVAO);
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		}
	}

	void EnvironmentMap::BakeBRDF(uint32 fbo, uint32 screenVAO, usize indexCount)
	{
		const int32 res = 512;
		m_brdf = MGF_RESOURCE.CreateImmediate<GLTexture2D>(res, res, GL_RG16F, 1);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, m_brdf->GetHandle(), 0);
		glViewport(0, 0, res, res);

		m_brdfProgram->Use();

		glBindVertexArray(screenVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}