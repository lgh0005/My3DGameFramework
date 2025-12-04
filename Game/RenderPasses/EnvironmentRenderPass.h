#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(EnvironmentRenderPass)
class EnvironmentRenderPass : public GeneralRenderPass
{
public:
    static EnvironmentRenderPassUPtr Create
    (
        ProgramUPtr program,     
        CubeTexturePtr cubeTexture
    );

    virtual void Render(Scene* scene, Camera* camera) override;

    const std::vector<MeshRenderer*>& GetRenderers() const;
    void AddRenderer(MeshRenderer* meshRenderer);

private:
    EnvironmentRenderPass() = default;
    bool Init
    (
        ProgramUPtr program,
        CubeTexturePtr cubeTexture
    );

    CubeTexturePtr m_cubeTexture;
    ProgramUPtr m_envProgram;
    std::vector<MeshRenderer*> m_renderers;
};