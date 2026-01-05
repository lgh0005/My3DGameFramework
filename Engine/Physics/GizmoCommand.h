#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
#pragma endregion

struct GizmoMeshCommand
{
    MeshPtr mesh;
    glm::mat4 worldMat;
    glm::vec4 color;
};

struct GizmoLineCommand
{
    glm::vec3 start;
    glm::vec3 end;
    glm::vec4 color;
};
