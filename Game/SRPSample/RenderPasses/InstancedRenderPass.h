#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(InstancedMeshRenderer)
#pragma endregion

CLASS_PTR(InstancedRenderPass)
class InstancedRenderPass : public GeneralRenderPass
{
public:
    static InstancedRenderPassUPtr Create(ProgramUPtr program);
    virtual void Render(Scene* scene, Camera* camera) override;

    const std::vector<InstancedMeshRenderer*>& GetRenderers() const;
    void AddRenderer(InstancedMeshRenderer* meshRenderer);

private:
    InstancedRenderPass() = default;
    bool Init(ProgramUPtr program);
    ProgramUPtr m_instanceProgram;

    std::vector<InstancedMeshRenderer*> m_renderers;
};