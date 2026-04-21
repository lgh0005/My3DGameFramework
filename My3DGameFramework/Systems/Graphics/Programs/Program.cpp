#include "GraphicsPch.h"
#include "Program.h"
#include "Assets/Shader.h"
#include "Shader/GLShader.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
    Program::Program(StringView name) : Super(name) { }
    Program::~Program()
    {
        if (m_handle != 0)
        {
            glDeleteProgram(m_handle);
            m_handle = 0;
        }
    }

    /*========================//
    //      Program Type      //
    //========================*/
    int16 Program::s_typeIndex = -1;
    const MGFType* Program::GetType() const
    {
        MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
        if (tree != nullptr) return tree->GetType(s_typeIndex);
        return nullptr;
    }

    void Program::AddShader(const ShaderPtr& shader)
    {
        if (shader) m_pendingShaders.push_back(shader);
    }

    bool Program::OnSyncCreate()
    {
        {
            // 1. 등록된 셰이더 개수 확인
            MGF_LOG_FATAL("Program Sync Check - Shader Count: {0}", m_pendingShaders.size());

            for (int i = 0; i < m_pendingShaders.size(); ++i)
            {
                auto& shader = m_pendingShaders[i];
                if (!shader) continue;

                // 2. 주소와 상태를 동시에 출력
                MGF_LOG_FATAL("Shader[{0}] Addr: {1}, State: {2}",
                    i, (void*)shader.get(), (int)shader->GetState());

                if (shader->GetState() != EAssetState::Ready)
                {
                    MGF_LOG_ERROR("Waiting for Shader[{0}] (Addr: {1}) to be Ready.", i, (void*)shader.get());
                    return false;
                }
            }

            if (m_pendingShaders.empty()) {
                MGF_LOG_FATAL("Program Pending: No Shaders Attached.");
                return false;
            }

            for (int i = 0; i < m_pendingShaders.size(); ++i)
            {
                auto& shader = m_pendingShaders[i];
                if (!shader) {
                    MGF_LOG_FATAL("Program Pending: Shader[{0}] is NULL.", i);
                    return false;
                }
                if (shader->GetState() != EAssetState::Ready) {
                    MGF_LOG_FATAL("Program Pending: Shader[{0}] is not Ready yet.", i);
                    return false;
                }

                // 리소스가 실제로 들어있는지도 체크
                if (shader->GetResources().empty()) {
                    MGF_LOG_FATAL("Program Pending: Shader[{0}] has NO resources.", i);
                    return false;
                }
            }
        }

        if (m_pendingShaders.empty()) return false;

        // 1. 의존성 체크: 부착할 모든 셰이더가 컴파일 완료(Ready) 상태인지 확인
        for (const auto& shader : m_pendingShaders)
        {
            if (!shader || shader->GetState() != EAssetState::Ready)
                return false;
        }

        // 2. [중요] 핸들이 없을 때만 딱 한 번 생성
        if (m_handle == 0) m_handle = glCreateProgram();

        for (auto& shader : m_pendingShaders)
        {
            auto glShader = MGFTypeCaster::Cast<GLShader>(shader->GetResources()[0]);
            if (!glShader || glShader->GetState() != EResourceState::Ready) return false;
            glAttachShader(m_handle, glShader->GetHandle());
        }

        glLinkProgram(m_handle);

        // 3. 링크 상태 검증
        int32 success = 0;
        glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(m_handle, 1024, nullptr, infoLog);
            MGF_LOG_ERROR("Failed to link program: {}", infoLog);
            m_state = EResourceState::Failed;
            glDeleteProgram(m_handle);
            m_handle = 0;
            return false;
        }

        // 4. 탈착 및 메모리 정리
        for (auto& shader : m_pendingShaders)
        {
            auto glShader = MGFTypeCaster::Cast<GLShader>(shader->GetResources()[0]);
            glDetachShader(m_handle, glShader->GetHandle());
        }

        m_pendingShaders.clear();
        m_pendingShaders.shrink_to_fit();

        m_state = EResourceState::Ready;
        return true;
    }

    void Program::Use() const
    {
        glUseProgram(m_handle);
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
        if (loc != -1) glProgramUniform1iv(m_handle, loc, static_cast<GLsizei>(value.size()), value.data());
    }

    void Program::SetUniform(const String& name, const Vector<mat4>& value)
    {
        if (value.empty()) return;
        int32 loc = GetUniformLocation(name);
        if (loc != -1) glProgramUniformMatrix4fv(m_handle, loc, static_cast<GLsizei>(value.size()), GL_FALSE, value_ptr(value[0]));
    }
}