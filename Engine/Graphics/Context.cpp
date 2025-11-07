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
    return std::move(context);
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Context::Init()
{
    // 1. 사각형 생성
    {
        float vertices[] =
        {
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        };

        uint32 indices[] =
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
    }

    // 2. 렌더링 파이프라인 생성
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
    }

    // 이미지 로드
    auto image1 = Image::Load("./Resources/Images/container.jpg");
    if (!image1)  return false;
    m_texture1 = Texture::CreateFromImage(image1.get());

    auto image2 = Image::Load("./Resources/Images/awesomeface.png");
    if (!image2)  return false;
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex1"), 0);
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);

    // Transform 행렬
    // 0.5배 축소후 z축으로 90도 회전하는 행렬
    auto transform = glm::rotate(
        glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)),
        glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)
    );
    auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    return true;
}