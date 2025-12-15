#include "EnginePch.h"
#include "Material.h"
#include "Graphics/Program.h"
#include "Graphics/Texture.h"

MaterialUPtr Material::Create()
{
    return MaterialUPtr(new Material());
}

void Material::SetToProgram(const Program* program) const 
{
    // 1. Albedo (Diffuse) - [Slot 0]
    // 기본값: White (색상 곱셈에 영향 없음)
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_ALBEDO);
    program->SetUniform("material.diffuse", (int)TextureSlot::SLOT_ALBEDO);
    if (diffuse) diffuse->Bind();
    else Texture::CreateWhite()->Bind();

    //// 2. Specular - [Slot 1]
    // 기본값: Black (반사광 없음)
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_SPECULAR);
    program->SetUniform("material.specular", (int)TextureSlot::SLOT_SPECULAR);
    if (specular) specular->Bind();
    else Texture::CreateBlack()->Bind();

    // 3. Emission - [Slot 2]
    // 기본값: Black (발광 없음)
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_EMISSION);
    program->SetUniform("material.emission", (int)TextureSlot::SLOT_EMISSION);
    if (emission) emission->Bind();
    else Texture::CreateBlack()->Bind();

    // 4. Normal - [Slot 3]
    // 기본값: Blue (RGB: 128, 128, 255 -> Vector: 0, 0, 1)
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_NORMAL);
    program->SetUniform("material.normal", (int)TextureSlot::SLOT_NORMAL);
    if (normal) normal->Bind();
    else Texture::CreateBlue()->Bind();

    // 5. Height (Displacement) - [Slot 4]
    // 기본값: Black (높이 변화 없음), 높이 스케일은 0
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_HEIGHT);
    program->SetUniform("material.height", (int)TextureSlot::SLOT_HEIGHT);
    if (height)
    {
        height->Bind();
        program->SetUniform("material.heightScale", heightScale);
    }
    else
    {
        Texture::CreateWhite()->Bind();
        program->SetUniform("material.heightScale", 0.0f);
    }

    // 6. ORM vs. AO, Matellic, Roughness
    if (orm)
    {
        // 6. Occulsion Roughness Metallic Map (ORM) - [Slot 8]
        // 기본값: Yello (차폐 없음, 완전 거침, 비금속)
        glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_ORM);
        program->SetUniform("material.orm", (int)TextureSlot::SLOT_ORM);
        orm->Bind();
        program->SetUniform("material.hasORM", true);
    }
    else
    {
        // 6. Ambient Occlusion (AO) - [Slot 5]
        // 기본값: White (차폐 없음, 모든 빛을 다 받음)
        glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_AO);
        program->SetUniform("material.ao", (int)TextureSlot::SLOT_AO);
        if (ao) ao->Bind();
        else Texture::CreateWhite()->Bind();

        // 7. Metallic - [Slot 6]
        // 기본값: Black (비금속)
        glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_METALLIC);
        program->SetUniform("material.metallic", (int)TextureSlot::SLOT_METALLIC);
        if (metallic) metallic->Bind();
        else Texture::CreateBlack()->Bind();

        // 8. Roughness - [Slot 7]
        // 기본값: Gray (0.5 - 적당한 거칠기) 혹은 White (완전 거침)
        glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_ROUGHNESS);
        program->SetUniform("material.roughness", (int)TextureSlot::SLOT_ROUGHNESS);
        if (roughness) roughness->Bind();
        else Texture::CreateGray()->Bind();

        program->SetUniform("material.hasORM", false);
    }

    glActiveTexture(GL_TEXTURE0);
    program->SetUniform("material.shininess", shininess);
    program->SetUniform("material.emissionStrength", emissionStrength);
    program->SetUniform("material.heightScale", heightScale);
    program->SetUniform("material.albedoFactor", albedoFactor);
    program->SetUniform("material.metallicFactor", metallicFactor);
    program->SetUniform("material.roughnessFactor", roughnessFactor);
    program->SetUniform("material.useGlossiness", useGlossinessAsRoughness);
}
