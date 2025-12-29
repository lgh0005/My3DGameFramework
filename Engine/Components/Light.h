#pragma once
#include "Component.h"

enum class LightType
{
    Directional,
    Point,
    Spot
};

CLASS_PTR(Light)
class Light : public Component
{
public:
    virtual ~Light();
    const glm::vec3& GetAmbient() const { return m_ambient; }
    const glm::vec3& GetDiffuse() const { return m_diffuse; }
    const glm::vec3& GetSpecular() const { return m_specular; }
    const float      GetIntensity() const { return m_intensity; }
    bool  IsCastShadow() const { return m_castShadow; }
    int32 GetShadowMapIndex() const { return m_shadowMapIndex; }
    const glm::mat4& GetLightSpaceMatrix() const { return m_lightSpaceMatrix; }

    void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
    void SetDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
    void SetSpecular(const glm::vec3& specular) { m_specular = specular; }
    void SetIntensity(float intensity) { m_intensity = intensity; }
    void SetCastShadow(bool cast) { m_castShadow = cast; }
    void SetShadowMapIndex(int32 index) { m_shadowMapIndex = index; }
    void SetLightSpaceMatrix(const glm::mat4& matrix) { m_lightSpaceMatrix = matrix; }

protected:
    Light();
    glm::vec3 m_ambient         { glm::vec3(0.1f, 0.1f, 0.1f) };
    glm::vec3 m_diffuse         { glm::vec3(0.5f, 0.5f, 0.5f) };
    glm::vec3 m_specular        { glm::vec3(1.0f, 1.0f, 1.0f) };
    float     m_intensity       { 1.0f };
    bool      m_castShadow      { false };
    int32     m_shadowMapIndex  { -1 };
    glm::mat4 m_lightSpaceMatrix{ glm::mat4(1.0f) };
};