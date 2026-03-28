#include "GraphicsPch.h"
#include "Shader.h"
#include "Stream/MemoryStream.h"
#include "Managers/StreamManager.h"

namespace MGF3D
{
    /*==================================//
    //         Type System              //
    //==================================*/
    UniquePtr<MGFType> Shader::s_type = nullptr;

    void Shader::InitializeType()
    {
        s_type = MakeUnique<MGFType>();
        s_type->name = MGFName("Shader");
        s_type->id = StringHash("Shader");

        // 부모(Asset)의 타입 참조
        const MGFType* parentType = Super::s_type.Get();
        if (parentType)
        {
            s_type->parent = parentType;
            s_type->depth = parentType->depth + 1;

            // 족보(Chain) 복사
            for (uint32 i = 0; i <= parentType->depth; ++i)
                s_type->chain[i] = parentType->chain[i];
        }

        // 족보의 끝에 내 ID 추가
        s_type->chain[s_type->depth] = s_type->id;
    }

    const MGFType* Shader::GetType() const
    {
        return nullptr;
    }

    /*==================================//
    //         Asset Logic              //
    //==================================*/
    Shader::Shader(const ShaderDescriptor& desc) : Super(desc)
    {
        auto shaderDesc = static_cast<const ShaderDescriptor*>(&desc);
        if (shaderDesc)
        {
            m_type = shaderDesc->shaderType;
            m_isSpirv = shaderDesc->isSpirv;
            m_entryPoint = shaderDesc->entryPoint;
        }
    }

    Shader::~Shader()
    {
        if (m_shader)
        {
            glDeleteShader(m_shader);
            m_shader = 0;
        }
    }

    ShaderPtr Shader::Create(ShaderDescriptor&& desc)
    {
        ShaderPtr shader = ShaderPtr(new Shader(desc));
        if (!shader->Init(std::move(desc))) return nullptr;
        return shader;
    }

    bool Shader::Init(ShaderDescriptor&& desc)
    {
        m_type = desc.shaderType;
        m_isSpirv = desc.isSpirv;
        m_entryPoint = std::move(desc.entryPoint);
        if (m_type == GL_NONE) return false;
        return true;
    }

    bool Shader::OnLoad()
    {
        m_sourceCode.Clear();

        // 파일 읽기는 Worker Thread에서 수행 (동기식 API 사용)
        auto result = MGF_STREAM.ReadAllBytes(m_path);
        if (result.IsNull())
        {
            MGF_LOG_ERROR("Shader OnLoad failed: Cannot read file '{}'", m_path.GetCStr());
            return false;
        }

        auto memoryStream = result.Get();
        usize len = memoryStream->GetLength();

        if (m_isSpirv)
        {
            m_binaryData.Resize(len);
            if (memoryStream->Read(m_binaryData.Data(), len) != len) return false;
        }
        else
        {
            m_sourceCode.Resize(len + 1); // 문자열 파싱을 위한 여유 공간
            if (memoryStream->Read(m_sourceCode.Data(), len) != len) return false;

            // 텍스트 셰이더의 경우 null-terminated 보장
            m_sourceCode.Data()[len] = '\0';
        }

        return true;
    }

    bool Shader::OnCommit()
    {
        // OpenGL API 호출은 Main Thread에서 수행
        m_shader = glCreateShader(m_type);

        // 1. 미리 컴파일된 셰이더인지 판별 후 따로 동작
        if (m_isSpirv)
        {
            if (m_binaryData.Empty()) return false;
            glShaderBinary(1, &m_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, m_binaryData.Data(), static_cast<GLsizei>(m_binaryData.Count()));
            glSpecializeShader(m_shader, (const GLchar*)m_entryPoint.CStr(), 0, nullptr, nullptr);
        }
        else
        {
            if (m_sourceCode.Empty()) return false;
            cstr codePtr = m_sourceCode.CStr();
            int32 codeLength = (int32)m_sourceCode.Length();
            glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
            glCompileShader(m_shader);
        }

        int32 success = 0;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char8 infoLog[1024];
            glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
            MGF_LOG_ERROR("Failed to compile shader: \"{}\"\nReason: {}", m_path.GetCStr(), infoLog);
            return false;
        }

        return true;
    }

    void Shader::OnRelease()
    {
        m_sourceCode.Clear();
        m_binaryData.Clear();
    }
}