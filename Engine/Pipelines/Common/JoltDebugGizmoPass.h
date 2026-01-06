#pragma once
#include "Graphics/RenderPass.h"
#include "Graphics/Vertex.h"
#include "Physics/GizmoCommand.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(DebugMesh)
#pragma endregion

CLASS_PTR(JoltDebugGizmoPass)
class JoltDebugGizmoPass : public ContextRenderPass
{
public:
	virtual ~JoltDebugGizmoPass();
	static JoltDebugGizmoPassUPtr Create(float lineWidth = 1.5f);
	virtual void Render(RenderContext* context) override;

/*=============================//
//   gizmo rendering methods   //
//=============================*/
private:
	void RenderDebugLines(const std::vector<GizmoLineCommand>& commands);
	void RenderDebugTriangles(const std::vector<GizmoTriangleCommand>& commands);

private:
	JoltDebugGizmoPass();
	bool Init(float lineWidth);

	ProgramPtr m_debugProgram;

	DebugMeshUPtr m_lineMesh;
	DebugMeshUPtr m_triMesh;

	float m_lineWidth	{ 1.5f };
	std::vector<GizmoVertex> m_lineBatch;
	std::vector<GizmoVertex> m_triBatch;
};