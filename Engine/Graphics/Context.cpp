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

    // 이미지로부터 텍스쳐를 생성
    m_texture = Texture::CreateFromImage(image.get());

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

        auto texLoc = glGetUniformLocation(m_program->Get(), "tex");
        glUniform1i(texLoc, 0);
    }

    return true;
}
