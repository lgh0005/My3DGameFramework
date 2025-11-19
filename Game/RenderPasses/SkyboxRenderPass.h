#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Mesh)
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(SkyboxRenderPass)
class SkyboxRenderPass : public SkyboxPass
{
    using Super = RenderPass;

public:
    static SkyboxRenderPassUPtr Create(ProgramUPtr program, 
                                       MeshPtr boxMesh, 
                                       CubeTexturePtr cubeTexture);
    virtual void Render(Scene* scene, Camera* camera) override;

private:
    SkyboxRenderPass() = default;
    bool Init
    (
        ProgramUPtr program,
        MeshPtr boxMesh,
        CubeTexturePtr cubeTexture
    );
};