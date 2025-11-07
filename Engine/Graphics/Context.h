#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Buffer)
CLASS_PTR(VertexLayout)
CLASS_PTR(Texture)
CLASS_PTR(Transform)
CLASS_PTR(Camera)
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
    TextureUPtr m_texture1;
    TextureUPtr m_texture2;
    VertexLayoutUPtr m_vertexLayout;
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;
    TransformUPtr m_cubeTransform;

    CameraUPtr m_camera;

};