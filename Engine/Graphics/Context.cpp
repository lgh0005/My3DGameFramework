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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    auto view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f));
    auto model = glm::rotate(glm::mat4(1.0f),
        glm::radians((float)glfwGetTime() * 120.0f),
        glm::vec3(1.0f, 0.5f, 0.0f));
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

bool Context::Init()
{
    // 1. 큐브 생성
    {
        float vertices[] = {
          -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
           0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
           0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
          -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

          -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
           0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
           0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
          -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

          -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
          -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
          -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
          -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

           0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
           0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
           0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
           0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

          -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
           0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
           0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
          -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

          -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
           0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
           0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
          -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
        };

        uint32 indices[] = {
           0,  2,  1,  2,  0,  3,
           4,  5,  6,  6,  7,  4,
           8,  9, 10, 10, 11,  8,
          12, 14, 13, 14, 12, 15,
          16, 17, 18, 18, 19, 16,
          20, 22, 21, 22, 20, 23,
        };

        m_vertexLayout = VertexLayout::Create();
        m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 120);
        m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
        m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 3 * sizeof(float));
        m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32) * 36);
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
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    // Transform 행렬
    //// 일반적인 변환 : 0.5배 축소후 z축으로 90도 회전하는 행렬
    //auto transform = glm::rotate(
    //    glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)),
    //    glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)
    //);

    //// 카메라를 반영한 변환
    // x축으로 -55도 회전
    auto model = glm::rotate(glm::mat4(1.0f),
        glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // 카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
    auto view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f));
    // 종횡비 4:3, 세로화각 45도의 원근 투영
    auto projection = glm::perspective(glm::radians(45.0f),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);


    return true;
}