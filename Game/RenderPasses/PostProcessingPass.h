#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Framebuffer)
#pragma endregion

CLASS_PTR(PostProcessingRenderPass)
class PostProcessingRenderPass : public RenderPass
{
    using Super = RenderPass;

public:
    static PostProcessingRenderPassUPtr Create
    (
        ProgramUPtr program,
        int32 width, int32 height,
        MeshPtr planeMesh
    );
    virtual void Render(Scene* scene, Camera* camera) override;
    void BeginDraw();

private:
    PostProcessingRenderPass() = default;
    bool Init(ProgramUPtr program, int32 width, int32 height, MeshPtr planeMesh);
    MeshPtr m_plane;
    FramebufferUPtr m_frameBuffer;
    float m_gamma       { 0.75f };
};