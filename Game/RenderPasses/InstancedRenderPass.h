#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(MeshRenderer)
#pragma endregion

CLASS_PTR(InstancedRenderPass)
class InstancedRenderPass : public GeneralRenderPass
{
public:
    static InstancedRenderPassUPtr Create(ProgramUPtr program);
    virtual void Render(Scene* scene, Camera* camera) override;

    const std::vector<MeshRenderer*>& GetRenderers() const;
    void AddRenderer(MeshRenderer* meshRenderer);

private:
    InstancedRenderPass() = default;
    bool Init(ProgramUPtr program);
    ProgramUPtr m_instanceProgram;
    std::vector<MeshRenderer*> m_renderers;
};