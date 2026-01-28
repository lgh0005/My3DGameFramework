#pragma once
#include "Resources/Resource.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Shader)
#pragma endregion

CLASS_PTR(Program)
class Program : public Resource
{
    DEFINE_RESOURCE_TYPE(ResourceType::Shader)

public:
    static ProgramUPtr Create(const std::vector<ShaderPtr>& shaders);
    static ProgramUPtr Create(const std::string& vertShaderFilename,
                              const std::string& fragShaderFilename);

    usize Get() const { return m_program; }
    void Use() const;
    ~Program();

/*==============================================//
//   default arithmetic value uniform setters   //
//==============================================*/
public:
    void SetUniform(const std::string& name, bool value) const;
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, const glm::vec2& value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;

/*==============================================//
//   default arithmetic value uniform setters   //
//==============================================*/
public:
    void SetUniform(const std::string& name, const std::vector<int>& value) const;
    void SetUniform(const std::string& name, const std::vector<glm::mat4>& value) const;
    void SetUniform(const std::string& name, const std::vector<glm::vec3>& value) const;

private:
    Program();
    bool Link(const std::vector<ShaderPtr>& shaders);
    usize m_program{ 0 };
};