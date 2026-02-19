#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Shader)
#pragma endregion

CLASS_PTR(Program)
class Program : public Resource
{
public:
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

/*===================================//
//   default array uniform setters   //
//===================================*/
public:
    void SetUniform(const std::string& name, const std::vector<int32>& value) const;
    void SetUniform(const std::string& name, const std::vector<glm::mat4>& value) const;
    void SetUniform(const std::string& name, const std::vector<glm::vec3>& value) const;

protected:
    Program();
    bool Link(const std::vector<ShaderPtr>& shaders);
    bool AddShaderStage
    (
        const std::string& path, GLenum type, 
        std::vector<ShaderPtr>& outShaders
    );

    usize m_program  { 0 };
};