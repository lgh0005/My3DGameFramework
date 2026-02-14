#include "EnginePch.h"
#include "Material.h"
#include "Parsers/EngineParsers/MaterialYamlParser.h"
#include "Resources/Programs/Program.h"
#include "Resources/Programs/GraphicsProgram.h"
#include "Resources/Textures/TextureUtils.h"
#include "Resources/Textures/Texture.h"

DECLARE_DEFAULTS_IMPL(Material)

MaterialPtr Material::Load(const MaterialDesc& desc)
{
    // 1. 머티리얼 파일 확장자 확인 (예: .mat, .json, .yaml)
    auto path = desc.path;
    std::string ext = std::filesystem::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".yaml")
    {
        LOG_ERROR("Material::Load - Invalid extension: {}", path);
        return nullptr;
    }

    // 2. 머티리얼 생성
    MaterialPtr material(new Material());
    material->m_desc = desc;
    
    // 3. 머티리얼 파일 파싱
    MaterialYamlParser parser;
    if (parser.LoadConfig(desc.path))
    {
        if (parser.LoadMaterial(material.get()))
            LOG_INFO("Material Loaded: {}", desc.path);
    }

    return material;
}

MaterialPtr Material::Create()
{
    // 1. 머티리얼 인스턴스 생성 (생성자는 protected이므로 내부에서 new)
    MaterialPtr material(new Material());

    MaterialDesc defaultDesc;
    defaultDesc.name = "DefaultMaterial";
    defaultDesc.path = "@BuiltIn/Material/Default";
    material->m_desc = defaultDesc;

    // 2. TextureUtils를 이용한 기본 텍스처 할당
    material->diffuse = TextureUtils::GetWhiteTexture(); // 색상 영향 없음
    material->specular = TextureUtils::GetBlackTexture(); // 반사 없음
    material->emission = TextureUtils::GetBlackTexture(); // 발광 없음
    material->normal = TextureUtils::GetBlueTexture();  // 평평한 노멀 (0, 0, 1)
    material->ao = TextureUtils::GetWhiteTexture(); // 차폐 없음
    material->metallic = TextureUtils::GetBlackTexture(); // 비금속
    material->roughness = TextureUtils::GetGrayTexture();  // 적당한 거칠기 (0.5)

    // 3. 기본 팩터 설정
    material->albedoFactor = glm::vec4(1.0f);
    material->emissiveFactor = glm::vec3(0.0f);
    material->metallicFactor = 1.0f;
    material->roughnessFactor = 1.0f;
    material->shininess = 32.0f;

    return material;
}

// INFO : 현재 이 SetToProgram은 머티리얼 관련 유니폼 변수를 확정적으로 강제하는
// 것임. 이는 엔진 외부 단에서도 반드시 지켜야 하는 유니폼 작성 포멧임을 유념해야 함.
// 포멧은 여타 다른 내부 엔진용 셰이더 파일에서 머티리얼 부분을 어떻게 선언하고 있는지 참고 바람.
void Material::SetToProgram(const Program* program) const 
{
    // 1. Albedo (Diffuse) - [Slot 0]
    // 기본값: White (색상 곱셈에 영향 없음)
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_ALBEDO);
    program->SetUniform("material.diffuse", (int)TextureSlot::SLOT_ALBEDO);
    if (diffuse) diffuse->Bind();
    else TextureUtils::GetWhiteTexture()->Bind();

    //// 2. Specular - [Slot 1]
    // 기본값: Black (반사광 없음)
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_SPECULAR);
    program->SetUniform("material.specular", (int)TextureSlot::SLOT_SPECULAR);
    if (specular) specular->Bind();
    else TextureUtils::GetBlackTexture()->Bind();

    // 3. Emission - [Slot 2]
    // 기본값: Black (발광 없음)
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_EMISSION);
    program->SetUniform("material.emission", (int)TextureSlot::SLOT_EMISSION);
    if (emission) emission->Bind();
    else TextureUtils::GetBlackTexture()->Bind();

    // 4. Normal - [Slot 3]
    // 기본값: Blue (RGB: 128, 128, 255 -> Vector: 0, 0, 1)
    glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_NORMAL);
    program->SetUniform("material.normal", (int)TextureSlot::SLOT_NORMAL);
    if (normal) normal->Bind();
    else TextureUtils::GetBlueTexture()->Bind();

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
        TextureUtils::GetWhiteTexture()->Bind();
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
        else TextureUtils::GetWhiteTexture()->Bind();

        // 7. Metallic - [Slot 6]
        // 기본값: Black (비금속)
        glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_METALLIC);
        program->SetUniform("material.metallic", (int)TextureSlot::SLOT_METALLIC);
        if (metallic) metallic->Bind();
        else TextureUtils::GetBlackTexture()->Bind();

        // 8. Roughness - [Slot 7]
        // 기본값: Gray (0.5 - 적당한 거칠기) 혹은 White (완전 거침)
        glActiveTexture(GL_TEXTURE0 + (int)TextureSlot::SLOT_ROUGHNESS);
        program->SetUniform("material.roughness", (int)TextureSlot::SLOT_ROUGHNESS);
        if (roughness) roughness->Bind();
        else TextureUtils::GetGrayTexture()->Bind();

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
