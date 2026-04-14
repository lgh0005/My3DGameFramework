#include "GraphicsPch.h"
#include "GLShader.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
    GLShader::GLShader() = default;
    GLShader::~GLShader()
    {
        if (m_handle != 0)
        {
            glDeleteShader(m_handle);
            m_handle = 0;
        }
    }

    /*========================//
    //     GLShader Type      //
    //========================*/
    int16 GLShader::s_typeIndex = -1;
    const MGFType* GLShader::GetType() const
    {
        MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
        if (tree != nullptr) return tree->GetType(s_typeIndex);
        return nullptr;
    }

    bool GLShader::OnSyncCreate()
    {
        // 1. 셰이더 생성
        m_handle = glCreateShader(m_type);

        if (!m_isSpirv)
        {
            // 2. GLSL 텍스트 컴파일
            if (m_sourceGLSL.empty()) return false;

            cstr src = m_sourceGLSL.c_str();
            glShaderSource(m_handle, 1, &src, nullptr);
            glCompileShader(m_handle);

            // 성공 여부 체크
            int32 success = 0;
            glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                char infoLog[1024];
                glGetShaderInfoLog(m_handle, 1024, nullptr, infoLog);
                MGF_LOG_ERROR("GLSL Compilation failed:\n{}", infoLog);
                return false;
            }

            m_sourceGLSL.clear();
            m_sourceGLSL.shrink_to_fit();
        }
        else
        {
            if (m_sourceSpirv.empty()) return false;

            // 바이너리 주입
            glShaderBinary(1, &m_handle, GL_SHADER_BINARY_FORMAT_SPIR_V,
                m_sourceSpirv.data(), static_cast<GLsizei>(m_sourceSpirv.size()));

            // 진입점 지정 및 컴파일 (특수화)
            glSpecializeShader(m_handle, m_entryPoint.c_str(), 0, nullptr, nullptr);

            // 성공 여부 체크 (SPIR-V도 COMPILE_STATUS로 확인)
            int32 success = 0;
            glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                char infoLog[1024];
                glGetShaderInfoLog(m_handle, 1024, nullptr, infoLog);
                MGF_LOG_ERROR("SPIR-V Specialization failed:\n{}", infoLog);
                return false;
            }

            // [핵심] 컴파일 완료 후 메모리 반환
            m_sourceSpirv.clear();
            m_sourceSpirv.shrink_to_fit();
            m_entryPoint.clear();
            m_entryPoint.shrink_to_fit();
        }

        m_state = EResourceState::Ready;
        return true;
    }

    void GLShader::SetSourceGLSL(GLenum type, const String& source)
    {
        m_type = type;
        m_sourceGLSL = source;
        m_isSpirv = false;
    }

    void GLShader::SetSourceSpirv(GLenum type, Vector<char>&& binary, const String& entryPoint)
    {
        m_type = type;
        m_sourceSpirv = std::move(binary);
        m_entryPoint = entryPoint;
        m_isSpirv = true;
    }
}