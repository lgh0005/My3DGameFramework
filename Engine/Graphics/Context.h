#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();
    ~Context();

private:
    Context() = default;
    bool Init();
    ProgramUPtr m_program;

// TEMP : 잠시 테스트
private:
    TextureUPtr m_texture;
    VertexLayoutUPtr m_vertexLayout;
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;
};