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

    // TEMP : 이후 씬과 컨텍스트를 통합한 구조 구축 필요
    Camera& GetCamera() { return *m_camera; }
    Framebuffer& GetFrameBuffer() { return *m_frameBuffer; }

private:
    Context() = default;
    bool Init();

private:
    // --- ImGui 제어용 임시 변수 ---
    Camera* m_camera{ nullptr };
    SpotLight* m_spotLight{ nullptr };

    // 카메라 제어용 변수
    glm::vec4 m_clearColor{ glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };
    float m_cameraPitch{ -20.0f };
    float m_cameraYaw{ 0.0f };
    bool  m_cameraControl{ false };
    glm::vec2 m_prevMousePos{ glm::vec2(0.0f) };

    // --- 후처리(Post-Processing) 리소스 ---
    FramebufferUPtr m_frameBuffer; // [추가] 씬 렌더링용 FBO
    MeshUPtr		m_plane;       // [추가] 후처리용 사각형
    ProgramUPtr		m_postProgram; // [추가] 후처리 셰이더
    float			m_gamma{ 1.0f }; // [추가] 감마 값
};
