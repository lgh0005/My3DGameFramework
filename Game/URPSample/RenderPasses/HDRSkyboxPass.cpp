#include "pch.h"
#include "HDRSkyboxPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Material.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Core/GameObject.h"

HDRSkyboxPassUPtr HDRSkyboxPass::Create(ProgramUPtr program)
{
    auto pass = HDRSkyboxPassUPtr(new HDRSkyboxPass());
    if (!pass->Init(std::move(program))) return nullptr;
    return std::move(pass);
}

bool HDRSkyboxPass::Init(ProgramUPtr program)
{
    m_skyboxProgram = std::move(program);
    if (!m_skyboxProgram) return false;

    m_cubeMesh = GeometryGenerator::CreateBox();
    if (!m_cubeMesh) return false;

    return true;
}

void HDRSkyboxPass::Render(Scene* scene, Camera* camera)
{
    // 1. 스카이박스 텍스쳐가 없으면 그릴 수 없음
    auto skyboxTexture = scene->GetSkyboxTexture();
    if (!skyboxTexture) return;

    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);

    m_skyboxProgram->Use();

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();

    m_skyboxProgram->SetUniform("view", view);
    m_skyboxProgram->SetUniform("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    m_skyboxProgram->SetUniform("cubeMap", 0);
    skyboxTexture->Bind();

    m_cubeMesh->Draw(m_skyboxProgram.get());

    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
}