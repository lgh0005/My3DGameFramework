#include "pch.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

Mesh::Mesh(const string& name) : Super(name, Resources::ResourceType::Mesh)
{

}

#pragma region PRIMITIVE_SHAPES
void Mesh::CreateTriangle()
{
    _geometry = make_shared<Geometry<VertexColorData>>();
    vector<VertexColorData> vertices = 
    {
        {{0.0f,  0.5f, 0.0f}, {1,0,0,1}},
        {{-0.5f, -0.5f, 0.0f}, {0,1,0,1}},
        {{0.5f, -0.5f, 0.0f}, {0,0,1,1}}
    };
    vector<GLuint> indices = { 0, 1, 2 };

    _geometry->SetVertices(vertices);
    _geometry->SetIndices(indices);
    CreateBuffers();
}

void Mesh::CreateQuad()
{
    _geometry = make_shared<Geometry<VertexColorData>>();
    vector<VertexColorData> vertices = 
    {
        {{-0.5f, -0.5f, 0.0f}, {1,0,0,1}},
        {{0.5f, -0.5f, 0.0f}, {0,1,0,1}},
        {{0.5f, 0.5f, 0.0f}, {0,0,1,1}},
        {{-0.5f, 0.5f, 0.0f}, {1,1,0,1}}
    };
    vector<GLuint> indices = { 0, 1, 2, 2, 3, 0 };

    _geometry->SetVertices(vertices);
    _geometry->SetIndices(indices);
    CreateBuffers();
}

void Mesh::CreatePyramid()
{
    _geometry = make_shared<Geometry<VertexColorData>>();
    vector<VertexColorData> vertices = 
    {
        {{-0.5f, -0.5f, 0.5f}, {1,0,0,1}},  
        {{0.5f, -0.5f, 0.5f}, {0,1,0,1}},  
        {{0.5f, -0.5f, -0.5f}, {0,0,1,1}},  
        {{-0.5f, -0.5f, -0.5f}, {1,1,0,1}}, 
        {{0.0f, 0.5f, 0.0f}, {1,0,1,1}}   
    };

    vector<GLuint> indices = 
    {
        0,1,4,
        1,2,4,
        2,3,4,
        3,0,4,
        0,3,2,
        2,1,0
    };

    _geometry->SetVertices(vertices);
    _geometry->SetIndices(indices);
    CreateBuffers();
}

void Mesh::CreateCube()
{
    _geometry = make_shared<Geometry<VertexColorData>>();

    vector<VertexColorData> vertices = 
    {
        {{-0.5f,-0.5f,-0.5f}, {1,0,0,1}}, //0
        {{0.5f,-0.5f,-0.5f}, {0,1,0,1}},  //1
        {{0.5f,0.5f,-0.5f}, {0,0,1,1}},   //2
        {{-0.5f,0.5f,-0.5f}, {1,1,0,1}},  //3
        {{-0.5f,-0.5f,0.5f}, {1,0,1,1}},  //4
        {{0.5f,-0.5f,0.5f}, {0,1,1,1}},   //5
        {{0.5f,0.5f,0.5f}, {1,1,1,1}},    //6
        {{-0.5f,0.5f,0.5f}, {0,0,0,1}}    //7
    };

    vector<GLuint> indices = 
    {
        0,1,2, 2,3,0, // back
        4,5,6, 6,7,4, // front
        4,0,3, 3,7,4, // left
        1,5,6, 6,2,1, // right
        3,2,6, 6,7,3, // top
        4,5,1, 1,0,4  // bottom
    };

    _geometry->SetVertices(vertices);
    _geometry->SetIndices(indices);

    CreateBuffers();
}

void Mesh::CreateSphere()
{
    _geometry = make_shared<Geometry<VertexColorData>>();

    float radius = 0.5f;
    uint32 stackCount = 20;
    uint32 sliceCount = 20;

    vector<VertexColorData> vertices;
    vector<GLuint> indices;

    // ∫œ±ÿ
    vertices.push_back({ {0.0f, radius, 0.0f}, {1,0,0,1} });

    float stackStep = glm::pi<float>() / stackCount;
    float sliceStep = glm::two_pi<float>() / sliceCount;

    // ¡ﬂ∞£ Ω∫≈√
    for (uint32 i = 1; i < stackCount; ++i)
    {
        float phi = i * stackStep;
        for (uint32 j = 0; j <= sliceCount; ++j)
        {
            float theta = j * sliceStep;
            VertexColorData v;
            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);
            v.color = { 1,1,1,1 }; // ¥‹º¯ »Úªˆ
            vertices.push_back(v);
        }
    }

    // ≥≤±ÿ
    vertices.push_back({ {0.0f, -radius, 0.0f}, {1,0,0,1} });

    // ∫œ±ÿ ªÔ∞¢«¸
    for (uint32 i = 1; i <= sliceCount; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // ∏ˆ≈Î
    uint32 ringVertexCount = sliceCount + 1;
    for (uint32 i = 0; i < stackCount - 2; ++i)
    {
        for (uint32 j = 0; j < sliceCount; ++j)
        {
            uint32 a = 1 + i * ringVertexCount + j;
            uint32 b = a + 1;
            uint32 c = a + ringVertexCount;
            uint32 d = c + 1;

            indices.push_back(a); indices.push_back(b); indices.push_back(c);
            indices.push_back(c); indices.push_back(b); indices.push_back(d);
        }
    }

    // ≥≤±ÿ ªÔ∞¢«¸
    uint32 southPoleIndex = static_cast<uint32>(vertices.size() - 1);
    uint32 lastRingStart = southPoleIndex - ringVertexCount;
    for (uint32 i = 0; i < sliceCount; ++i)
    {
        indices.push_back(southPoleIndex);
        indices.push_back(lastRingStart + i + 1);
        indices.push_back(lastRingStart + i);
    }

    _geometry->SetVertices(vertices);
    _geometry->SetIndices(indices);

    CreateBuffers();
}
#pragma endregion

void Mesh::CreateBuffers()
{
    _vao = std::make_shared<VertexArray>();
	_vertexBuffer = make_shared<VertexBuffer>();
	_vertexBuffer->Create(_geometry->GetVertices());
	_indexBuffer = make_shared<IndexBuffer>();
	_indexBuffer->Create(_geometry->GetIndices());

    _vao->AddVertexBuffer(_vertexBuffer, { 3, 4 });
    _vao->SetIndexBuffer(_indexBuffer);
}

