#include "../pch.h"
#include "DevRenderer.h"

#include "Core/Scene.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"

DevRendererUPtr DevRenderer::Create(int32 width, int32 height)
{
	auto devRenderer = DevRendererUPtr(new DevRenderer());
	if (!devRenderer->Init(width, height)) return nullptr;
	return std::move(devRenderer);
}

void DevRenderer::Render(Scene* scene)
{
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f); // 임시 clear color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	auto* camera = scene->GetActiveCamera();
	if (!camera) return;

	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();

	if (!m_simpleProgram) return; // 셰이더가 로드되지 않았으면 종료
	m_simpleProgram->Use();

	auto meshes = scene->GetAllMeshes();
	for (const auto* meshRenderer : meshes)
	{
		// 5. 렌더링에 필요한 컴포넌트 및 리소스 가져오기
		auto mesh = meshRenderer->GetMesh();
		auto& transform = meshRenderer->GetTransform();
		auto material = meshRenderer->GetMaterial(); // 재질도 가져옴

		// 6. 리소스 유효성 검사
		if (!mesh || !material) continue;

		// 7. MVP (Model-View-Projection) 행렬 계산
		auto model = transform.GetModelMatrix();
		auto mvp = projection * view * model;

		// 8. 셰이더에 유니폼(Uniform) 설정
		m_simpleProgram->SetUniform("transform", mvp);

		// [참고] m_simpleProgram은 텍스처를 사용하지 않고 단색만 출력합니다.
		// DevScene에서 container.jpg 텍스처를 설정했지만,
		// 이 셰이더를 사용하면 노란색으로 덮어씌워집니다.
		m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)); // 임시 노란색

		// 9. 그리기
		mesh->Draw(m_simpleProgram.get());
	}
}

bool DevRenderer::Init(int32 width, int32 height)
{
	m_simpleProgram = Program::Create
	(
		"./Resources/Shaders/simple.vert",
		"./Resources/Shaders/simple.frag"
	);
	if (!m_simpleProgram) return false;

	return true;
}

