#include "EnginePch.h"
#include "Program.h"
#include "Graphics/Shader.h"

Program::Program() = default;
Program::~Program()
{
    if (m_program) glDeleteProgram(m_program);
}

void Program::Use() const
{
    glUseProgram(m_program);
}

bool Program::Link(const std::vector<ShaderPtr>& shaders)
{
    m_program = glCreateProgram();

    // 1. Attach shaders to program
    for (auto& shader : shaders)
        glAttachShader(m_program, shader->Get());
    glLinkProgram(m_program);

    // 2. check program
    int32 success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        LOG_ERROR("failed to link program: {}", infoLog);
        return false;
    }

    // 3. Detach shaders
    for (auto& shader : shaders)
        glDetachShader(m_program, shader->Get());

    return true;
}

bool Program::AddShaderStage
(
    const std::string& path, GLenum type, 
    std::vector<ShaderPtr>& outShaders
)
{
    // 1. 경로가 없으면 그냥 통과
    if (path.empty()) return true;

    // 2. 셰이더 파일로부터 객체 생성
    auto shader = Shader::CreateFromFile(path, type);
    if (!shader) return false;

    // 3. 성공 시 목록에 추가
    outShaders.push_back(std::move(shader));
    return true;
}

/*==============================================//
//   default arithmetic value uniform setters   //
//==============================================*/
void Program::SetUniform(const std::string& name, bool value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(loc, static_cast<int>(value));
}

void Program::SetUniform(const std::string& name, int32 value) const 
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(loc, value);
}

void Program::SetUniform(const std::string& name, float value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(loc, value);
}

void Program::SetUniform(const std::string& name, const glm::vec2& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform2fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::vec3& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::vec4& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::mat4& value) const 
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const std::vector<int32>& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1iv(loc, (GLsizei)value.size(), value.data());
}

void Program::SetUniform(const std::string& name, const std::vector<glm::mat4>& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(loc, (GLsizei)value.size(), GL_FALSE, glm::value_ptr(value[0]));
}

void Program::SetUniform(const std::string& name, const std::vector<glm::vec3>& values) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(loc, (GLsizei)values.size(), glm::value_ptr(values[0]));
}