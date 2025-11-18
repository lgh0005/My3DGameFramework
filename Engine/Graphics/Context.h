#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Mesh)
CLASS_PTR(VertexLayout)
CLASS_PTR(Texture)
CLASS_PTR(Transform)
CLASS_PTR(Camera)
CLASS_PTR(Model)
CLASS_PTR(Material)
CLASS_PTR(Framebuffer)

CLASS_PTR(PointLight)
CLASS_PTR(DirectionalLight)
CLASS_PTR(SpotLight)
CLASS_PTR(Animator)
#pragma endregion

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Update();
    void Render();
    ~Context();

    // TEMP : 이후에 카메라 컨트롤러로 따로 작성 필요
    // + Input 매니저도 필요
    void ProcessInput(GLFWwindow* window);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

private:
    Context() = default;
    bool Init();

private:
    // TEMP : ImGui 제어용 임시 변수
    Camera* m_camera{ nullptr };
    SpotLight* m_spotLight{ nullptr };

    // TEMP : 카메라 제어용 변수
    glm::vec4 m_clearColor{ glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };
    float m_cameraPitch{ -20.0f };
    float m_cameraYaw{ 0.0f };
    bool  m_cameraControl{ false };
    glm::vec2 m_prevMousePos{ glm::vec2(0.0f) };
};
