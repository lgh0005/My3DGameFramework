#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
CLASS_PTR(Program)
#pragma endregion

/*==================//
//  material enums  //
//==================*/
enum class TextureSlot : int8
{
    SLOT_ALBEDO = 0,
    SLOT_SPECULAR = 1,
    SLOT_EMISSION = 2,
    SLOT_NORMAL = 3,
    SLOT_HEIGHT = 4,

    // TODO : 이후에 ORM 텍스쳐 하나로 합쳐야 한다.
    SLOT_AO = 5,
    SLOT_METALLIC = 6,
    SLOT_ROUGHNESS = 7
};

/*==================//
//  material class  //
//==================*/
CLASS_PTR(Material);
class Material 
{
public:
    static MaterialUPtr Create();

    TexturePtr  diffuse;  // [공통]
    TexturePtr  specular; // [SRP]
    TexturePtr  emission; // [공통]
    TexturePtr  normal;   // [공통]
    TexturePtr  height;   // [공통]

    // TODO : 이후에 ORM 텍스쳐 하나로 합쳐야 한다.
    TexturePtr  ao;       // [공통]
    TexturePtr  metallic; // [URP]
    TexturePtr  roughness;

    float       shininess           { 32.0f }; // [SRP]
    float       emissionStrength    { 1.0f };  // [공통]
    float       heightScale         { 1.0f };  // [공통]

    // TODO : 머티리얼에 albedoFactor, metallicFactor,
    // roughnessFactor를 추가할 필요가 있음
    /*float       albedoFactor;
    float       metallicFactor;
    float       roughnessFactor*/;

    void SetToProgram(const Program* program) const;

protected:
    Material() = default;
};