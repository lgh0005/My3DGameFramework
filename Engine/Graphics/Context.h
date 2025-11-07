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

    // TEMP : 이후에 카메라 컨트롤러로 따로 작성 필요
    // + Input 매니저도 필요
    void ProcessInput(GLFWwindow* window);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

    // TEMP : Reshape -> 이후에 window 매니저가 따로 처리 필요
    // 카메라의 perspective도 같이 수정을 해줘야 하는 것에 유의.
    // 지금 당장에서는 윈도우 크기를 줄일 때 찌그러 지는 것에 대한
    // 처리는 생략하겠음.
    Camera& GetCamera() { return *m_camera; }

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

    TransformUPtr m_cubeTransform1;
    TransformUPtr m_cubeTransform2;

    CameraUPtr m_camera;
    // TODO : 이후에 camera controller와 같은 곳에서
    // 처리해야 할 내용
    float m_cameraPitch         { 0.0f };
    float m_cameraYaw           { 0.0f };
    bool  m_cameraControl       { false };
    glm::vec2 m_prevMousePos    { glm::vec2(0.0f) };
};
