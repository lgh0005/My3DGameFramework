#pragma once
#include "GizmoCommand.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(JoltGizmo)
class JoltGizmo
{
public:
    ~JoltGizmo();
    static JoltGizmoUPtr Create();

    void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
    void DrawTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec4& color);

    const std::vector<GizmoLineCommand>& GetLineCommands() const { return m_lineCommands; }
    const std::vector<GizmoTriangleCommand>& GetTriangleCommands() const { return m_triangleCommands; }

    void Clear();

private:
    JoltGizmo();

    std::vector<GizmoLineCommand> m_lineCommands;
    std::vector<GizmoTriangleCommand> m_triangleCommands;
    MeshPtr     m_cubeMesh;
    MeshPtr     m_sphereMesh;
};