#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(Material);
class Material {
public:
    static MaterialUPtr Create() { return MaterialUPtr(new Material()); }
    TexturePtr  diffuse;
    TexturePtr  specular;
    float       shininess { 32.0f };

    void SetToProgram(const Program* program) const;

private:
    Material() = default;
};