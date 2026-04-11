#include "GraphicsPch.h"
#include "Shader.h"

namespace MGF3D
{
    Shader::Shader() = default;
    Shader::~Shader()
    {
        if (m_handle != 0)
        {
            glDeleteShader(m_handle);
            m_handle = 0;
        }
    }

    ShaderPtr Shader::Create
    (
        const String& filename, GLenum shaderType, 
        EShaderFileType shaderFileType, const String& entryPoint
    )
    {
        // 1. 인스턴스 생성
        auto shader = ShaderPtr(new Shader());

        bool success = false;
        switch (shaderFileType)
        {
        case EShaderFileType::GLSL: success = shader->LoadSpirV(filename, shaderType, entryPoint); break;
        case EShaderFileType::SPIRV: success = shader->LoadGLSL(filename, shaderType); break;
        }

        // 3. 실패 시 nullptr 반환
        if (!success) return nullptr;
        return shader;
    }

    bool Shader::LoadGLSL(const String& filename, GLenum shaderType)
    {
        // 1. 파일 읽기 로직 직접 포함
        InputFileStream fin(filename.c_str());
        if (!fin.is_open())
        {
            MGF_LOG_ERROR("Failed to open GLSL file: {}", filename.c_str());
            return false;
        }

        StringStream text;
        text << fin.rdbuf();
        String code = text.str();
        if (code.empty())
        {
            MGF_LOG_ERROR("GLSL file is empty: {}", filename.c_str());
            return false;
        }

        cstr codePtr = code.c_str();
        int32 codeLength = static_cast<int32>(code.length());

        // 2. 셰이더 생성 및 컴파일
        m_handle = glCreateShader(shaderType);
        glShaderSource(m_handle, 1, &codePtr, &codeLength);
        glCompileShader(m_handle);

        // 3. 컴파일 결과 확인
        int32 success = 0;
        glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(m_handle, 1024, nullptr, infoLog);
            MGF_LOG_ERROR("Failed to compile GLSL: {}\nReason: {}", filename.c_str(), infoLog);
            return false;
        }

        return true;
    }

    bool Shader::LoadSpirV(const String& filename, GLenum shaderType, const String& entryPoint)
    {
        // 1. 바이너리 파일 읽기
        InputFileStream file(filename.c_str(), std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            MGF_LOG_ERROR("Failed to open SPIR-V file: {}", filename.c_str());
            return false;
        }

        usize fileSize = static_cast<usize>(file.tellg());
        if (fileSize == 0) return false;

        Vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        // 2. 셰이더 생성 및 바이너리 주입
        m_handle = glCreateShader(shaderType);
        glShaderBinary(1, &m_handle, GL_SHADER_BINARY_FORMAT_SPIR_V, buffer.data(), static_cast<GLsizei>(buffer.size()));
        
        // 3. 진입점 설정 (Specialization)
        glSpecializeShader(m_handle, entryPoint.c_str(), 0, nullptr, nullptr);

        // 4. 로드 상태 확인
        int32 success = 0;
        glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(m_handle, 1024, nullptr, infoLog);
            MGF_LOG_ERROR("Failed to load SPIR-V: {}\nReason: {}", filename.c_str(), infoLog);
            return false;
        }

        return true;
    }
}