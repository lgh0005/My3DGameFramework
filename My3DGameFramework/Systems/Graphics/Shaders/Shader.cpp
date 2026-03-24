#include "GraphicsPch.h"
#include "Shader.h"

namespace MGF3D
{
    Shader::Shader() = default;
    Shader::~Shader()
    {
        if (m_shader)
        {
            glDeleteShader(m_shader);
            m_shader = 0;
        }
    }

    ShaderPtr Shader::CreateFromTextFile(const SString& filename, GLenum shaderType)
    {
        auto shader = ShaderPtr(new Shader());
        if (!shader->InitFromTextFile(filename, shaderType)) return nullptr;
        return shader;
    }

    ShaderPtr Shader::CreateFromSPIRV(const SString& filename, GLenum shaderType, strview entryPoint)
    {
        auto shader = ShaderPtr(new Shader());
        if (!shader->InitFromSPIRV(filename, shaderType, entryPoint)) return nullptr;
        return shader;
    }

    Nullable<SString> Shader::LoadTextFile(const SString& filename)
    {
        return Nullable<SString>();
    }

    Nullable<SVector<uint8>> Shader::LoadBinaryFile(const SString& filename)
    {
        return Nullable<SVector<uint8>>();
    }

    bool Shader::InitFromTextFile(const SString& filename, GLenum shaderType)
    {
        auto result = LoadTextFile(filename);
        if (!result.IsValid())
            return false;

        auto& code = result.Get();
        cstr codePtr = code.CStr();
        int32 codeLength = (int32)code.Length();

        // create and compile shader
        m_shader = glCreateShader(shaderType);
        glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
        glCompileShader(m_shader);

        // check compile error
        int success = 0;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
            MGF_LOG_ERROR("failed to compile shader: \"{}\"", filename);
            MGF_LOG_ERROR("reason: {}", infoLog);
            return false;
        }
        return true;
    }

    bool Shader::InitFromSPIRV(const SString& filename, GLenum shaderType, strview entryPoint)
    {
        return false;
    }
}