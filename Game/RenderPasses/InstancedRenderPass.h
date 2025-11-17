#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(InstancedRenderPass)
class InstancedRenderPass : public RenderPass
{
public:
    static InstancedRenderPassUPtr Create(ProgramUPtr program);
    virtual void Render(Scene* scene, Camera* camera) override;

private:
    InstancedRenderPass() = default;
};