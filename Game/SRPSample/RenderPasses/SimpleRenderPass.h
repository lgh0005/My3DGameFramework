#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(StaticMeshRenderer)
#pragma endregion

CLASS_PTR(SimpleRenderPass)
class SimpleRenderPass : public GeneralRenderPass
{
public:
    virtual ~SimpleRenderPass();
    static SimpleRenderPassUPtr Create(ProgramUPtr program);
    virtual void Render(Scene* scene, Camera* camera) override;

    const std::vector<StaticMeshRenderer*>& GetRenderers() const;
    void AddRenderer(StaticMeshRenderer* meshRenderer);

private:
    SimpleRenderPass();
    bool Init(ProgramUPtr program);
    ProgramUPtr m_simpleProgram;
    std::vector<StaticMeshRenderer*> m_renderers;
};