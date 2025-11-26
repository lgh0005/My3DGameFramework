#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Framebuffer)
#pragma endregion

CLASS_PTR(PostProcessingRenderPass)
class PostProcessingRenderPass : public PostProcessPass
{
    using Super = PostProcessPass;

public:
    static PostProcessingRenderPassUPtr Create
    (
        ProgramUPtr finalCompositeProgram,
        ProgramUPtr blurProgram,
        int32 width, int32 height,
        MeshPtr planeMesh
    );
    virtual void Render(Scene* scene, Camera* camera) override;

private:
    PostProcessingRenderPass() = default;
    bool Init
    (
        ProgramUPtr finalCompositeProgram,
        ProgramUPtr blurProgram,
        int32 width, int32 height, 
        MeshPtr planeMesh
    );

    ProgramUPtr		m_blurProgram;
    // FramebufferUPtr m_pingPongFBOs[2];
};