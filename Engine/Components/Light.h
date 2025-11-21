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
    static const ComponentType s_ComponentType = ComponentType::Light;
    virtual ComponentType GetType() const override { return ComponentType::Light; }
    virtual LightType GetLightType() const = 0;

    const glm::vec3& GetAmbient() const { return m_ambient; }
    const glm::vec3& GetDiffuse() const { return m_diffuse; }
    const glm::vec3& GetSpecular() const { return m_specular; }

    void SetAmbient(const glm::vec3& ambient) { m_ambient = ambient; }
    void SetDiffuse(const glm::vec3& diffuse) { m_diffuse = diffuse; }
    void SetSpecular(const glm::vec3& specular) { m_specular = specular; }

protected:
    Light() = default;
    glm::vec3 m_ambient{ glm::vec3(0.1f, 0.1f, 0.1f) };
    glm::vec3 m_diffuse{ glm::vec3(0.5f, 0.5f, 0.5f) };
    glm::vec3 m_specular{ glm::vec3(1.0f, 1.0f, 1.0f) };
    // TODO m_intensity를 추가해야 할까?
};