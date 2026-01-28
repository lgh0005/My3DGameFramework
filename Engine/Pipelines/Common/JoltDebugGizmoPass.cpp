#include "EnginePch.h"
#include "Components/Camera.h"
#include "JoltDebugGizmoPass.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
#include "Resources/Meshes/DebugMesh.h"
#include "Physics/JoltGizmo.h"

DECLARE_DEFAULTS_IMPL(JoltDebugGizmoPass)

JoltDebugGizmoPassUPtr JoltDebugGizmoPass::Create(float lineWidth)
{
	auto pass = JoltDebugGizmoPassUPtr(new JoltDebugGizmoPass());
	if (!pass->Init(lineWidth)) return nullptr;
	return std::move(pass);
}

bool JoltDebugGizmoPass::Init(float lineWidth)
{
	// 0. 멤버 변수 대입
	m_lineWidth = lineWidth;

	// 1. 쉐이더 로드 (이전 단계에서 만든 Gizmo.glsl)
	// 이름은 리소스 매니저에 등록된 이름으로 맞춰주세요.
	m_debugProgram = RESOURCE.GetResource<GraphicsProgram>("debug_gizmos");
	if (!m_debugProgram) return false;

	// 2. 디버그 메쉬 생성
	// DebugMesh::Init 내부에서 빈 VBO 생성과 레이아웃 설정이 자동으로 수행됨
	m_lineMesh = DebugMesh::Create(GL_LINES);
	m_triMesh = DebugMesh::Create(GL_TRIANGLES);

	// 3. 배칭 벡터 미리 예약 (메모리 재할당 최소화)
	m_lineBatch.reserve(10000);
	m_triBatch.reserve(10000);

	return true;
}

void JoltDebugGizmoPass::Render(RenderContext* context)
{
	// 1. Jolt 물리 시스템에게 "현재 상태를 그려달라"고 요청
	// 이 함수가 실행되면 내부적으로 JoltDebugRenderer -> JoltGizmo 순으로 데이터가 쌓임
	PHYSICS.DrawDebugData();

	JoltGizmo* gizmo = PHYSICS.GetGizmo();
	if (!gizmo) return;

	const auto& lineCmds = gizmo->GetLineCommands();
	const auto& triCmds = gizmo->GetTriangleCommands();
	if (lineCmds.empty() && triCmds.empty()) return;

	m_debugProgram->Use();

	// 렌더링
	glEnable(GL_DEPTH_TEST);
	glLineWidth(m_lineWidth);

	RenderDebugLines(lineCmds);
	RenderDebugTriangles(triCmds);

	glLineWidth(1.0f);

	gizmo->Clear();
}

/*=============================//
//   gizmo rendering methods   //
//=============================*/
void JoltDebugGizmoPass::RenderDebugLines(const std::vector<GizmoLineCommand>& commands)
{
	if (!commands.empty())
	{
		m_lineBatch.clear();

		// 벡터 용량 확보 (부족하면 늘림)
		if (m_lineBatch.capacity() < commands.size() * 2)
			m_lineBatch.reserve(commands.size() * 2);

		// Command -> Vertex 변환
		for (const auto& cmd : commands)
		{
			m_lineBatch.push_back({ cmd.start, cmd.color });
			m_lineBatch.push_back({ cmd.end, cmd.color });
		}

		// GPU 전송 및 그리기
		m_lineMesh->UpdateGeometry(m_lineBatch);
		m_lineMesh->Draw();
	}
}

void JoltDebugGizmoPass::RenderDebugTriangles(const std::vector<GizmoTriangleCommand>& commands)
{
	if (!commands.empty())
	{
		m_triBatch.clear();

		if (m_triBatch.capacity() < commands.size() * 3)
			m_triBatch.reserve(commands.size() * 3);

		for (const auto& cmd : commands)
		{
			m_triBatch.push_back({ cmd.v1, cmd.color });
			m_triBatch.push_back({ cmd.v2, cmd.color });
			m_triBatch.push_back({ cmd.v3, cmd.color });
		}

		m_triMesh->UpdateGeometry(m_triBatch);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_triMesh->Draw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
