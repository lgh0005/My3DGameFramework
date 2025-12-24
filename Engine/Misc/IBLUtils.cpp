#include "EnginePch.h"
#include "IBLUtils.h"
#include "Graphics/Program.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/CubeTexture.h"
#include "Resources/Texture.h"
#include "Resources/ScreenMesh.h"
#include "Graphics/CubeFramebuffer.h"
#include "Graphics/FrameBuffer.h"

CubeTexturePtr IBLUtils::CreateCubemapFromHDR(Texture* hdrTexture, int32 resolution)
{
	// 일회용 유틸성 쉐이더 로드
	auto sphericalToCubeProgram = Program::Create
	(
		"./Resources/Shaders/IBLUtils/IBLUtils_Common.vert",
		"./Resources/Shaders/IBLUtils/IBLUtils_Spherical_Map.frag"
	);
	if (!sphericalToCubeProgram) return nullptr;

	// 일회용 큐브 메쉬 생성
	auto cubeMesh = GeometryGenerator::CreateBox();
	if (!cubeMesh) return nullptr;

	// 결과물을 담을 큐브맵 생성
	CubeTexturePtr envCubemap = CubeTexture::Create(resolution, resolution, GL_RGB16F, GL_FLOAT);
	if (!envCubemap) return nullptr;

	// 베이킹 준비 (FBO, 행렬 등)
	auto captureFBO = CubeFramebuffer::Create(envCubemap);
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	std::vector<glm::mat4> captureViews =
	{
		glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// 렌더링 (일회용 리소스 사용)
	sphericalToCubeProgram->Use();
	sphericalToCubeProgram->SetUniform("tex", 0);
	glActiveTexture(GL_TEXTURE0);
	hdrTexture->Bind();

	GLint prevViewport[4];
	glGetIntegerv(GL_VIEWPORT, prevViewport);
	glViewport(0, 0, resolution, resolution);
	captureFBO->Bind();

	glDisable(GL_CULL_FACE);
	for (int i = 0; i < 6; ++i)
	{
		sphericalToCubeProgram->SetUniform("transform", captureProjection * captureViews[i]);
		captureFBO->Bind(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubeMesh->Draw(sphericalToCubeProgram.get());
	}
	glEnable(GL_CULL_FACE);
	CubeFramebuffer::BindToDefault();
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

	// 6. 밉맵 생성
	envCubemap->GenerateMipmap();

	return envCubemap;
}

CubeTexturePtr IBLUtils::CreateIrradianceMap(CubeTexture* src)
{
	// 일회용 유틸성 쉐이더 로드
	auto convolutionProgram = Program::Create
	(
		"./Resources/Shaders/IBLUtils/IBLUtils_Common.vert",
		"./Resources/Shaders/IBLUtils/IBLUtils_Diffuse_Irradiance.frag"
	);
	if (!convolutionProgram) return nullptr;

	// 일회용 큐브 메쉬 생성
	auto cubeMesh = GeometryGenerator::CreateBox();
	if (!cubeMesh) return nullptr;

	// 결과물을 담을 큐브맵 생성
	int32 resolution = 64;
	CubeTexturePtr irradianceMap = CubeTexture::Create(resolution, resolution, GL_RGB16F, GL_FLOAT);
	if (!irradianceMap) return nullptr;

	// 베이킹 준비 (FBO, 행렬)
	auto captureFBO = CubeFramebuffer::Create(irradianceMap);
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	std::vector<glm::mat4> captureViews =
	{
		glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// 렌더링 (일회용 리소스 사용)
	convolutionProgram->Use();
	convolutionProgram->SetUniform("cubeMap", 0);
	glActiveTexture(GL_TEXTURE0);
	src->Bind();

	GLint prevViewport[4];
	glGetIntegerv(GL_VIEWPORT, prevViewport);
	glViewport(0, 0, resolution, resolution);

	captureFBO->Bind();
	glDisable(GL_CULL_FACE);
	for (int i = 0; i < 6; ++i)
	{
		convolutionProgram->SetUniform("transform", captureProjection * captureViews[i]);
		captureFBO->Bind(i);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cubeMesh->Draw(convolutionProgram.get());
	}
	glEnable(GL_CULL_FACE);
	CubeFramebuffer::BindToDefault();
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

	return irradianceMap;
}

CubeTexturePtr IBLUtils::CreatePrefilteredMap(CubeTexture* src)
{
	const uint32 maxMipLevels = 5;
	const uint32 baseResolution = 128;
	GLint prevViewport[4];
	glGetIntegerv(GL_VIEWPORT, prevViewport);

	// 일회용 유틸성 쉐이더 로드
	auto preFilteredProgram = Program::Create
	(
		"./Resources/Shaders/IBLUtils/IBLUtils_Common.vert",
		"./Resources/Shaders/IBLUtils/IBLUtils_Prefiltered_Light.frag"
	);
	if (!preFilteredProgram) return nullptr;

	auto cubeMesh = GeometryGenerator::CreateBox();
	if (!cubeMesh) return nullptr;


	CubeTexturePtr preFilteredMap = CubeTexture::Create(baseResolution, baseResolution, 
														     GL_RGB16F, GL_FLOAT);
	if (!preFilteredMap) return nullptr;
	preFilteredMap->GenerateMipmap();

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	std::vector<glm::mat4> captureViews =
	{
		glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	preFilteredProgram->Use();
	preFilteredProgram->SetUniform("cubeMap", 0);
	glActiveTexture(GL_TEXTURE0);
	src->Bind();

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	for (uint32 mip = 0; mip < maxMipLevels; mip++)
	{
		uint32 mipWidth = 128 >> mip;
		uint32 mipHeight = 128 >> mip;
		auto framebuffer = CubeFramebuffer::Create(preFilteredMap, mip);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		preFilteredProgram->SetUniform("roughness", roughness);
		for (uint32_t i = 0; i < 6; i++)
		{
			preFilteredProgram->SetUniform("transform", captureProjection * captureViews[i]);
			framebuffer->Bind(i);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cubeMesh->Draw(preFilteredProgram.get());
		}
	}
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	CubeFramebuffer::BindToDefault();
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

	return preFilteredMap;
}

TexturePtr IBLUtils::CreateBRDFLUT()
{
	const uint32 resolution = 512;
	GLint prevViewport[4];
	glGetIntegerv(GL_VIEWPORT, prevViewport);
	glViewport(0, 0, resolution, resolution);

	auto brdfProgram = Program::Create(
		"./Resources/Shaders/IBLUtils/IBLUtils_BRDF_Lookup.vert",
		"./Resources/Shaders/IBLUtils/IBLUtils_BRDF_Lookup.frag"
	);
	if (!brdfProgram) return nullptr;

	auto lookupFramebuffer = Framebuffer::CreateBRDFLUT(resolution, resolution);
	if (!lookupFramebuffer) return nullptr;

	lookupFramebuffer->Bind();

	// TEMP : brdfLookUpTexture를 만들기 위한 하드코드
	// TODO : 이후에 이를 따로 ScreenMesh로 빼야함.
	// TODO : 이후 ScreenMesh가 가져야할 로직
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);

	brdfProgram->Use();

	// 스크린 메쉬
	auto screen = ScreenMesh::Create();
	screen->Draw();

	// 복구
	Framebuffer::BindToDefault();
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);

	return lookupFramebuffer->GetColorAttachment(0);
}
