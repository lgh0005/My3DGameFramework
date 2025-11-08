#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Shader)
#pragma endregion

CLASS_PTR(Program)
class Program
{
public:
    static ProgramUPtr Create(const std::vector<ShaderPtr>& shaders);
    static ProgramUPtr Create(const std::string& vertShaderFilename,
                              const std::string& fragShaderFilename);

    usize Get() const { return m_program; }
    void Use() const;
    ~Program();

public:
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;


private:
    Program() = default;
    bool Link(const std::vector<ShaderPtr>& shaders);
    usize m_program{ 0 };
};