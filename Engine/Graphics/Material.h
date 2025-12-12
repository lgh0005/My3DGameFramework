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
    SLOT_AO = 5,
    SLOT_METALLIC = 6,
    SLOT_ROUGHNESS = 7
};

enum class DefaultMaterialType { SRP, URP };

/*==============================//
//  material common properties  //
//==============================*/
CLASS_PTR(Material);
class Material 
{
public:
    static MaterialUPtr Create(); // TODO : 이후에 추상 클래스로 둬야함.

    TexturePtr  diffuse;  // [공통]
    TexturePtr  specular; // [SRP]
    TexturePtr  emission; // [공통]
    TexturePtr  normal;   // [공통]
    TexturePtr  height;   // [공통]
    TexturePtr  ao;       // [공통]
    TexturePtr  metallic; // [URP]
    TexturePtr  roughness;// [URP]
    float       shininess           { 32.0f }; // [SRP]
    float       emissionStrength    { 1.0f };  // [공통]
    float       heightScale         { 1.0f };  // [공통]

    virtual void SetToProgram(const Program* program) const;
    // virtual DefaultMaterialType GetType() const = 0;

protected:
    Material() = default;
};

#pragma region MATERIALS_NOW_WORKING
/*===============================//
//  material properties for SRP  //
//===============================*/
CLASS_PTR(StandardMaterial)
class StandardMaterial : public Material
{
    using Super = Material;

public:
    static StandardMaterialUPtr Create();
    TexturePtr  specular; // [SRP]
    float       shininess{ 32.0f }; // [SRP]
    virtual void SetToProgram(const Program* program) const override;

private:
    StandardMaterial() = default;
};

/*===============================//
//  material properties for URP  //
//===============================*/
CLASS_PTR(UniversalMaterial)
class UniversalMaterial : public Material
{
    using Super = Material;

public:
    static UniversalMaterialUPtr Create();
    TexturePtr  metallic; // [URP]
    TexturePtr  roughness;// [URP]
    virtual void SetToProgram(const Program* program) const override;

private:
    UniversalMaterial() = default;
};
#pragma endregion