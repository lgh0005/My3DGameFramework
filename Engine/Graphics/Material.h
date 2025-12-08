#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
CLASS_PTR(Program)
#pragma endregion

enum class TextureSlot : int8
{
    SLOT_ALBEDO = 0,
    SLOT_SPECULAR = 1,
    SLOT_EMISSION = 2,
    SLOT_NORMAL = 3,
    SLOT_HEIGHT = 4,
    SLOT_AO = 5,
    SLOT_METALLIC = 6,
    SLOT_ROUGHNESS = 7
};

CLASS_PTR(Material);
class Material 
{
public:
    static MaterialUPtr Create();

    TexturePtr  diffuse;
    TexturePtr  specular;
    TexturePtr  emission;
    TexturePtr  normal;
    TexturePtr  height;
    TexturePtr  ao;
    TexturePtr  metallic;
    TexturePtr  roughness;
    float       shininess           { 32.0f };
    float       emissionStrength    { 1.0f };
    float       heightScale         { 1.0f };

    void SetToProgram(const Program* program) const;

private:
    Material() = default;
};