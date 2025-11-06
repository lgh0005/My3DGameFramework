#include "EnginePch.h"
#include "Context.h"
#include "Graphics/Shader.h"
#include "Graphics/Program.h"
#include "Graphics/Buffer.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Image.h"
#include "Graphics/Texture.h"

Context::~Context() = default;

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init()) return nullptr;
    return move(context);
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Context::Init()
{
    ShaderPtr vertShader = Shader::CreateFromFile("./Resources/Shaders/texture.vert", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./Resources/Shaders/texture.frag", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader) return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({ fragShader, vertShader });
    if (!m_program) return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // 이미지 로드
    auto image = Image::Load("./Resources/Images/container.jpg");
    if (!image)  return false;
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    // 이미지로부터 텍스쳐를 생성 및 바인딩
    m_texture = Texture::CreateFromImage(image.get());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    auto texLoc = glGetUniformLocation(m_program->Get(), "tex");
    glUniform1i(texLoc, 0);

    // 사각형 그리기
    {
        float vertices[] = 
        {
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        };
        uint32_t indices[] = 
        { 
            0, 1, 3, // first triangle
            1, 2, 3, // second triangle
        };

        m_vertexLayout = VertexLayout::Create();
        m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 32);
        m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
        m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 3 * sizeof(float));
        m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 6 * sizeof(float));
        m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32) * 6);

        // TEMP : Transform
        m_program->Use();
        auto transform = glm::mat4(1.0f);
        auto transformloc = glGetUniformLocation(m_program->Get(), "transform");

        glm::vec3 scale(1.0f, 1.0f, 1.0f);
        auto S = glm::scale(glm::mat4(1.0f), scale);

        // Rotation (예: Z축 기준 45도 회전)
        float angle = glm::radians(45.0f);
        auto R = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

        // Translation
        glm::vec3 pos(0.5f, 0.5f, 0.0f);
        auto T = glm::translate(glm::mat4(1.0f), pos);

        // SRT 결합 (T * R * S 순서 중요)
        transform = T * R * S;

        glUniformMatrix4fv(transformloc, 1, GL_FALSE, glm::value_ptr(transform));
    }

    return true;
}
