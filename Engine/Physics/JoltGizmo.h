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

    void DrawCube(const glm::vec3& center, const glm::vec3& size, const glm::vec4& color);
    void DrawSphere(const glm::vec3& center, float radius, const glm::vec4& color);
    void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);

    const std::vector<GizmoMeshCommand>& GetMeshCommands() const { return m_meshCommands; }
    const std::vector<GizmoLineCommand>& GetLineCommands() const { return m_lineCommands; }

    void Clear();

private:
    JoltGizmo();
    bool Init();

    std::vector<GizmoMeshCommand> m_meshCommands;
    std::vector<GizmoLineCommand> m_lineCommands;
    MeshPtr     m_cubeMesh;
    MeshPtr     m_sphereMesh;
};