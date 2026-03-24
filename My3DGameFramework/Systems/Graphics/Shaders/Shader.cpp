#include "GraphicsPch.h"
#include "Shader.h"
#include "Managers/StreamManager.h"
#include "Managers/PathManager.h"

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

    /*===================================//
    //   Shader loading asynchronously   //
    //===================================*/
    void Shader::CreateFromTextFileAsync(const MGFPath& filepath, GLenum shaderType, Action<ShaderPtr> onComplete)
    {
        // 1. 비동기 파일 읽기 시작
        MGF_STREAM.ReadFileAsync<SString>
        (
            filepath,

            /* [Worker Thread] 파일을 읽어 문자열로 변환 */
            [](FileStreamPtr file) -> Nullable<SString>
            {
                usize len = static_cast<usize>(file->GetLength());
                SString content;
                content.Resize(len);
                if (file->Read(content.Data(), len) != len) return None;
                return content;
            },

            /* [Main Thread] 로드된 데이터를 OpenGL로 컴파일 */
            [shaderType, filepath, onComplete](Nullable<SString> result)
            {
                if (result.IsNull()) 
                {
                    onComplete(nullptr);
                    return;
                }

                auto shader = ShaderPtr(new Shader());
                if (shader->InitFromTextFile(result.Get(), shaderType, filepath)) onComplete(shader);
                else onComplete(nullptr);
            }
        );
    }

    void Shader::CreateFromSPIRVAsync(const MGFPath& filepath, GLenum shaderType, strview entryPoint, Action<ShaderPtr> onComplete)
    {
        MGF_STREAM.ReadFileAsync<SVector<byte>>
        (   
            filepath,
            
            /* [Worker Thread] 바이너리 로드 */
            [](FileStreamPtr file) -> Nullable<SVector<byte>> 
            {
                usize len = static_cast<usize>(file->GetLength());
                SVector<byte> buffer;
                buffer.Resize(len);
                if (file->Read(buffer.Data(), len) != len) return None;
                return buffer;
            },

            /* [Main Thread] SPIR-V 전문화 */
            [shaderType, filepath, entryPoint = SString(entryPoint), onComplete](Nullable<SVector<byte>> result) 
            {
                if (result.IsNull()) 
                {
                    onComplete(nullptr);
                    return;
                }

                auto shader = ShaderPtr(new Shader());
                if (shader->InitFromBinary(result.Get(), shaderType, entryPoint, filepath))
                    onComplete(shader);
                else onComplete(nullptr);
            }
        );
    }

    /*==================================//
    //   Shader loading synchronously   //
    //==================================*/
    ShaderPtr Shader::CreateFromTextFile(const MGFPath& filepath, GLenum shaderType)
    {
        auto shader = ShaderPtr(new Shader());
        if (!shader->InitFromTextFile(filepath, shaderType)) return nullptr;
        return shader;
    }

    ShaderPtr Shader::CreateFromSPIRV(const MGFPath& filepath, GLenum shaderType, strview entryPoint)
    {
        auto shader = ShaderPtr(new Shader());
        if (!shader->InitFromSPIRV(filepath, shaderType, entryPoint)) return nullptr;
        return shader;
    }

    /*===================================//
    //   Shader Initialization methods   //
    //===================================*/
    bool Shader::InitFromTextFile(const MGFPath& filepath, GLenum shaderType)
    {
        auto result = LoadTextFile(filepath);
        if (!result.IsValid())
            return false;

        // 여기에 wait을 걸긴 해야 할 것 같은데 그러면 멀티스레드 의미가 있는걸까?

        const auto& code = result.Get();
        cstr codePtr = code.CStr();
        int32 codeLength = (int32)code.Length();

        // create and compile shader
        m_shader = glCreateShader(shaderType);
        m_type = shaderType;
        glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
        glCompileShader(m_shader);

        // check compile error
        int success = 0;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
            MGF_LOG_ERROR("failed to compile shader: \"{}\"", filepath);
            MGF_LOG_ERROR("reason: {}", infoLog);
            return false;
        }
        return true;
    }

    bool Shader::InitFromSPIRV(const MGFPath& filepath, GLenum shaderType, strview entryPoint)
    {
        auto result = LoadBinaryFile(filepath);
        if (!result.IsValid())
            return false;

        // 여기에 wait을 걸긴 해야 할 것 같은데 그러면 멀티스레드 의미가 있는걸까?

        const auto& binary = result.Get();
        m_shader = glCreateShader(shaderType);
        m_type = shaderType;

        // 1. 바이너리 데이터를 OpenGL에 전달
        glShaderBinary(1, &m_shader, GL_SHADER_BINARY_FORMAT_SPIR_V, binary.Data(), static_cast<GLsizei>(binary.Count()));
   
        // 2. SPIR-V 전문화 (Entry Point 지정)
        // glSpecializeShader는 SPIR-V의 'main' 함수나 상수를 설정합니다.
        glSpecializeShader(m_shader, (const GLchar*)entryPoint.data(), 0, nullptr, nullptr);

        // 3. 상태 체크
        int success = 0;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
            MGF_LOG_ERROR("failed to specialize SPIR-V shader: \"{}\"", filepath);
            MGF_LOG_ERROR("reason: {}", infoLog);
            return false;
        }

        MGF_LOG_INFO("Shader: SPIR-V loaded and specialized: {}", filepath.GetCStr());
        return true;
    }
}