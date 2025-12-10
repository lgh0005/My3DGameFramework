#include "EnginePch.h"
#include "IBLUtils.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/CubeFramebuffer.h"
#include "Graphics/Texture.h"

CubeTexturePtr IBLUtils::CreateCubemapFromHDR(Texture* hdrTexture, int32 resolution)
{
	// 일회용 유틸성 쉐이더 로드
	auto sphericalToCubeProgram = Program::Create
	(
		"./Resources/Shaders/Utils/spherical_map.vert",
		"./Resources/Shaders/Utils/spherical_map.frag"
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
	envCubemap->Bind();
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return envCubemap;
}