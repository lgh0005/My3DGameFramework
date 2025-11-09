#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)

CLASS_PTR(Mesh)
CLASS_PTR(VertexLayout)
CLASS_PTR(Texture)
CLASS_PTR(Transform)
CLASS_PTR(Camera)
CLASS_PTR(Model)

CLASS_PTR(PointLight)
CLASS_PTR(DirectionalLight)
CLASS_PTR(SpotLight)
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

    // 1. 기본 텍스쳐 프로그램
    ProgramUPtr m_program;       // 이미지를 텍스쳐로 출력
    ProgramUPtr m_simpleProgram; // 그냥 하나의 단색만 출력

    // 2. 조명 프로그램
    ProgramUPtr m_lighting;      // 단색에 조명 효과
    ProgramUPtr m_lighting2;     // 텍스쳐에 조명 효과

// TEMP : 잠시 테스트
private:
    // TODO : 이후 Mesh와 Material로 통합
    // cube properties
    TextureUPtr m_texture1;
    TextureUPtr m_texture2;
    MeshUPtr m_box;
    TransformUPtr m_cubeTransform1;
    TransformUPtr m_cubeTransform2;

    // model properties
    ModelUPtr m_model;
     
    // TODO : 이후에 camera controller와 같은 곳에서 처리해야 할 내용
    CameraUPtr m_camera;

    // camera parameters
    glm::vec4 m_clearColor      { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };
    float m_cameraPitch         { 0.0f };
    float m_cameraYaw           { 0.0f };
    bool  m_cameraControl       { false };
    glm::vec2 m_prevMousePos    { glm::vec2(0.0f) };

    // 임시 PointLight 컴포넌트
    PointLightUPtr       m_pointLight;
    DirectionalLightUPtr m_directionalLight;
    SpotLightPtr         m_spotLight;

    // TODO : 나중에 텍스쳐가 따로 지정된게 없다면
    // 기본 색상/값으로 들어가도록 처리해야 할 필요가 있을 것 같다.
    // Material : 텍스쳐를 입힌 머티리얼
    struct Material
    {
        TextureUPtr diffuse;
        TextureUPtr specular;
        float shininess{ 32.0f };
    };
    Material m_material;
};
