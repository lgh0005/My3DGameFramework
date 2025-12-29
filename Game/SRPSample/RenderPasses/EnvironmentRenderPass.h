#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(CubeTexture)
CLASS_PTR(StaticMeshRenderer)
#pragma endregion

CLASS_PTR(EnvironmentRenderPass)
class EnvironmentRenderPass : public GeneralRenderPass
{
public:
    virtual ~EnvironmentRenderPass();
    static EnvironmentRenderPassUPtr Create
    (
        ProgramUPtr program,     
        CubeTexturePtr cubeTexture
    );

    virtual void Render(Scene* scene, Camera* camera) override;

    const std::vector<StaticMeshRenderer*>& GetRenderers() const;
    void AddRenderer(StaticMeshRenderer* meshRenderer);

private:
    EnvironmentRenderPass();
    bool Init
    (
        ProgramUPtr program,
        CubeTexturePtr cubeTexture
    );

    CubeTexturePtr m_cubeTexture;
    ProgramUPtr m_envProgram;
    std::vector<StaticMeshRenderer*> m_renderers;
};