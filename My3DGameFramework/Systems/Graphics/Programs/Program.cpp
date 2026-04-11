#include "GraphicsPch.h"
#include "Program.h"
#include "Shader/Shader.h"

namespace MGF3D
{
    Program::Program() = default;
    Program::~Program()
    {
        if (m_handle != 0)
        {
            glDeleteProgram(m_handle);
            m_handle = 0;
        }
    }

    void Program::Use() const
    {
        glUseProgram(m_handle);
    }

    bool Program::Link(const Vector<ShaderPtr>& shaders)
    {
        // 0. 프로그램 생성
        m_handle = glCreateProgram();

        // 1. 프로그램에 셰이더 부착
        for (auto& shader : shaders) glAttachShader(m_handle, shader->GetHandle());
        glLinkProgram(m_handle);

        // 2. 프로그램 체크
        int32 success = 0;
        glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(m_handle, 1024, nullptr, infoLog);
            MGF_LOG_ERROR("failed to link program: {}", infoLog);
            return false;
        }

        // 3. 프로그램에 셰이더 탈착
        for (auto& shader : shaders)
            glDetachShader(m_handle, shader->GetHandle());

        return true;
    }

    int32 Program::GetUniformLocation(const String& name)
    {
        auto hash = StringHash(name);
        auto it = m_location.find(hash);
        if (it != m_location.end()) return it->second;

        int32 loc = glGetUniformLocation(m_handle, name.c_str());
        m_location[hash] = loc;
        if (loc == -1)
            MGF_LOG_WARN("Uniform '{}' not found in program {}", name.c_str(), m_handle);

        return loc;
    }

    /*==============================================//
    //    DSA 기반 유니폼 세터 (glProgramUniform)    //
    //==============================================*/
    void Program::SetUniform(const String& name, bool value)
    {
        glProgramUniform1i(m_handle, GetUniformLocation(name), static_cast<int32>(value));
    }

    void Program::SetUniform(const String& name, int32 value)
    {
        glProgramUniform1i(m_handle, GetUniformLocation(name), value);
    }

    void Program::SetUniform(const String& name, float value)
    {
        glProgramUniform1f(m_handle, GetUniformLocation(name), value);
    }

    void Program::SetUniform(const String& name, const vec2& value)
    {
        glProgramUniform2fv(m_handle, GetUniformLocation(name), 1, value_ptr(value));
    }

    void Program::SetUniform(const String& name, const vec3& value)
    {
        glProgramUniform3fv(m_handle, GetUniformLocation(name), 1, value_ptr(value));
    }

    void Program::SetUniform(const String& name, const vec4& value)
    {
        glProgramUniform4fv(m_handle, GetUniformLocation(name), 1, value_ptr(value));
    }

    void Program::SetUniform(const String& name, const mat4& value)
    {
        glProgramUniformMatrix4fv(m_handle, GetUniformLocation(name), 1, GL_FALSE, value_ptr(value));
    }

    /*===================================//
    //    default array uniform setters  //
    //===================================*/
    void Program::SetUniform(const String& name, const Vector<int32>& value)
    {
        if (value.empty()) return;
        int32 loc = GetUniformLocation(name);
        if (loc != -1)
        {
            glProgramUniform1iv(m_handle, loc, static_cast<GLsizei>(value.size()), value.data());
        }
    }

    void Program::SetUniform(const String& name, const Vector<mat4>& value)
    {
        if (value.empty()) return;
        int32 loc = GetUniformLocation(name);
        if (loc != -1)
        {
            glProgramUniformMatrix4fv(m_handle, loc,
                static_cast<GLsizei>(value.size()), GL_FALSE, value_ptr(value[0]));
        }
    }
}