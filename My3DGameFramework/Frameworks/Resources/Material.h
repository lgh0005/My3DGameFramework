#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
CLASS_PTR(Program)
#pragma endregion

/*==================//
//  material enums  //
//==================*/
enum class TextureSlot : uint8
{
    SLOT_ALBEDO = 0,
    SLOT_SPECULAR = 1,
    SLOT_EMISSION = 2,
    SLOT_NORMAL = 3,
    SLOT_HEIGHT = 4,
    SLOT_AO = 5,
    SLOT_METALLIC = 6,
    SLOT_ROUGHNESS = 7,
    SLOT_ORM = 8,
    SLOT_GLOSSINESS = 9
};

CLASS_PTR(Material);
class Material : public Resource
{
    DEFINE_RESOURCE_TYPE(ResourceType::Material, MaterialDesc)

public:
    virtual ~Material();
    static MaterialPtr Load(const MaterialDesc& desc);
    static MaterialPtr Create();
    virtual MaterialDesc& GetDesc() override { return m_desc; }
    virtual const ResourceDesc& GetDesc() const override { return m_desc; }

    /*=====================//
    //  material textures  //
    //=====================*/
    TexturePtr  diffuse;   // [Common]
    TexturePtr  specular;  // [SRP]
    TexturePtr  emission;  // [Common]
    TexturePtr  normal;    // [Common]
    TexturePtr  height;    // [Common]
    TexturePtr  ao;        // [Common]
    TexturePtr  metallic;  // [URP]
    TexturePtr  roughness; // [URP]
    TexturePtr  orm;       // [URP]

    /*====================//
    //  material factors  //
    //====================*/
    float       shininess           { 32.0f }; // [SRP]
    float       emissionStrength    { 1.0f };  // [Common]
    float       heightScale         { 1.0f };  // [Common]

    glm::vec4   albedoFactor        { 1.0f, 1.0f, 1.0f, 1.0f };  // [URP]
    glm::vec3   emissiveFactor      { 0.0f, 0.0f, 0.0f };
    float       metallicFactor      { 1.0f };  // [URP]
    float       roughnessFactor     { 1.0f };  // [URP]

    bool useGlossinessAsRoughness   { false }; // [URP]

    void SetToProgram(const Program* program) const;

private:
    Material();
    MaterialDesc m_desc;
};