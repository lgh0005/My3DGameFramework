#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(SimpleRenderPass)
class SimpleRenderPass : public RenderPass
{
public:
    static SimpleRenderPassUPtr Create(ProgramUPtr program);
    virtual void Render(Scene* scene, Camera* camera) override;

private:
    SimpleRenderPass() = default;
};