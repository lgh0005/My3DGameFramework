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

    // 1. 기본 텍스쳐 프로그램
    ProgramUPtr m_program;         // 이미지를 텍스쳐로 출력
    ProgramUPtr m_simpleProgram;   // 그냥 하나의 단색만 출력

    // 2. 조명 프로그램
    ProgramUPtr m_lighting;        // 단색에 조명 효과
    ProgramUPtr m_lighting2;       // 텍스쳐에 조명 효과

    // 3. 애니메이션 프로그램
    ProgramUPtr m_skinningProgram; // 스키닝 전용 셰이더

    // **[중요]** 4. 프레임 버퍼
    // TODO : 이후에는 이를 바탕으로 렌더링을 수행할 예정
    ProgramUPtr     m_postProgram;
    FramebufferUPtr m_frameBuffer;
    MeshUPtr        m_plane;
    float           m_gamma     { 1.0f };

// TEMP : 잠시 테스트
private:

    // 큐브 Mesh
    MeshUPtr m_box;
    MaterialPtr m_planeMaterial;
    MaterialPtr m_box1Material;
    MaterialPtr m_box2Material;
    MaterialPtr m_lightMaterial;

    // model properties
    ModelUPtr m_model;       // 애니메이팅 모델
    TransformUPtr m_modelTransform;
    AnimatorUPtr m_animator;

    // box properties
    TransformUPtr m_box1Transform;
    TransformUPtr m_box2Transform;
    TransformUPtr m_groundTransform;
     
    // TODO : 이후에 camera controller와 같은 곳에서 처리해야 할 내용
    CameraUPtr m_camera;

    // camera parameters
    glm::vec4 m_clearColor      { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };
    float m_cameraPitch         { -20.0f };
    float m_cameraYaw           { 0.0f };
    bool  m_cameraControl       { false };
    glm::vec2 m_prevMousePos    { glm::vec2(0.0f) };

    // 임시 조명 컴포넌트들
    PointLightUPtr              m_pointLight;
    DirectionalLightUPtr        m_directionalLight;
    SpotLightPtr                m_spotLight;
};
