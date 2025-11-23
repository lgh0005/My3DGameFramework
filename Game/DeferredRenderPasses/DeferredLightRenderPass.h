#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(DeferredLightPass)
class DeferredLightPass : public DeferredLightingPass
{
    using Super = RenderPass;

public:
    virtual ~DeferredLightPass();
    static DeferredLightPassUPtr Create
    (
        ProgramUPtr program, 
        MeshPtr screenMesh
    );
    virtual void Render(Scene* scene, Camera* camera) override;

private:
    DeferredLightPass() = default;
    bool Init(ProgramUPtr program, MeshPtr screenMesh);
};