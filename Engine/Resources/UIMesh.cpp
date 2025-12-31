#include "EnginePch.h"
#include "UIMesh.h"
#include "Resources/Material.h"
#include "Graphics/Buffer.h"
#include "Graphics/Vertex.h"
#include "Graphics/VertexLayout.h"
#include "Graphics/Program.h"

UIMesh::UIMesh() = default;
UIMesh::~UIMesh() = default;

UIMeshUPtr UIMesh::Create()
{
    UIMeshUPtr mesh = UIMeshUPtr(new UIMesh());
    mesh->Init();
    return mesh;
}

void UIMesh::Init()
{
    m_primitiveType = GL_TRIANGLES;

    // 0.0 ~ 1.0 범위의 Unit Quad 정점 데이터
    // 이렇게 하면 Transform Position이 곧 UI의 좌측 하단 좌표가 되어 배치가 쉽습니다.
    std::vector<ScreenVertex> vertices =
    {
        ScreenVertex { { 0.0f,  1.0f, 0.0f}, {0.0f, 1.0f} }, // 좌측 상단
        ScreenVertex { { 0.0f,  0.0f, 0.0f}, {0.0f, 0.0f} }, // 좌측 하단 (원점)
        ScreenVertex { { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} }, // 우측 상단
        ScreenVertex { { 1.0f,  0.0f, 0.0f}, {1.0f, 0.0f} }  // 우측 하단
    };

    // 컬링 영역 설정
    glm::vec3 minBound(FLT_MAX); // 아주 큰 값으로 초기화
    glm::vec3 maxBound(-FLT_MAX); // 아주 작은 값으로 초기화
    for (const auto& v : vertices)
    {
        minBound = Utils::Min(minBound, v.position);
        maxBound = Utils::Max(maxBound, v.position);
    }
    // UI의 기본 피벗은 좌측 하단이기에 중앙을 맞춘다.
    m_localBounds = RenderBounds::Create
    (
        (minBound + maxBound) * 0.5f,
        (maxBound - minBound) * 0.5f
    );

    // 인덱스 설정
    std::vector<uint32> indices =
    {
        0, 1, 2, // 첫 번째 삼각형
        2, 1, 3  // 두 번째 삼각형
    };

    m_indexCount = static_cast<uint32>(indices.size());

    // 1. 버퍼 생성 (VBO, IBO)
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData
    (
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(ScreenVertex), vertices.size()
    );
    m_indexBuffer = Buffer::CreateWithData
    (
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32), indices.size()
    );

    // 정점 속성 설정
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(ScreenVertex), offsetof(ScreenVertex, position));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(ScreenVertex), offsetof(ScreenVertex, texCoord));
}

void UIMesh::Draw() const
{
    m_vertexLayout->Bind();
    glDrawElements(m_primitiveType, m_indexCount, GL_UNSIGNED_INT, 0);
}

