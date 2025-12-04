#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(SimpleRenderPass)
class SimpleRenderPass : public GeneralRenderPass
{
public:
    static SimpleRenderPassUPtr Create(ProgramUPtr program);
    virtual void Render(Scene* scene, Camera* camera) override;

    const std::vector<MeshRenderer*>& GetRenderers() const;
    void AddRenderer(MeshRenderer* meshRenderer);

private:
    SimpleRenderPass() = default;
    bool Init(ProgramUPtr program);
    ProgramUPtr m_simpleProgram;
    std::vector<MeshRenderer*> m_renderers;
};