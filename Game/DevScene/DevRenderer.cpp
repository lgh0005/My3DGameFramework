#include "pch.h"
#include "DevRenderer.h"

#include "Core/Scene.h"
#include "Core/RenderPass.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/SkinnedMesh.h"
#include "Graphics/InstancedMesh.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Image.h"
#include "Graphics/ShadowMap.h"

#include "RenderPasses/PostProcessingPass.h"

DevRendererUPtr DevRenderer::Create(int32 width, int32 height)
{
	auto devRenderer = DevRendererUPtr(new DevRenderer());
	return std::move(devRenderer);
}

// TODO : 이후에는 이 Render의 동작을 SRP(StandartRenderPipeline)클래스로 따로 빼서
// 이 일련의 동작들을 하나의 클래스로 만들 예정.
void DevRenderer::Render(Scene* scene)
{
    // TEMP : UBO 테스트 중
    /*==============//
    //   test ubo   //
    //==============*/
    scene->PreRender();

	/*====================================//
	//   get essential scene properties   //
	//====================================*/
	// 메인 카메라 속성 가져오기
	auto* camera = scene->GetMainCamera();
	if (!camera) return;

	// 메인 조명 속성 가져오기
	SpotLight* mainLight = static_cast<SpotLight*>(scene->GetMainLight());

	/*================================//
	//   main scene rendergin logic   //
	//================================*/
	// [패스 1] 그림자 패스: m_shadowMap에 깊이 정보 기록
	scene->GetShadowPass()->Render(scene, camera);

	// [패스 2] 디퍼드 셰이딩 및 포스트 프로세스 프레임 버퍼에 깊이 복사
	scene->GetGeometryPass()->Render(scene, camera);
	scene->GetDeferredLightingPass()->Render(scene, camera);

	auto gBuffer = scene->GetGeometryPass()->GetGBuffer();
	auto postFBO = scene->GetPostProcessPass()->GetFramebuffer();
	BlitCopyDepth(gBuffer, postFBO, gBuffer->GetWidth(), gBuffer->GetHeight());

	// [패스 3] 포워드 셰이딩 
	for (const auto& [name, pass] : scene->GetRenderPasses())
		pass->Render(scene, camera);

	// [패스 4] 스카이박스 패스: m_frameBuffer에 스카이박스 덧그리기
	scene->GetSkyboxPass()->Render(scene, camera);

	// [패스 5] 후처리 패스: m_frameBuffer의 결과를 화면에 Resolve
	scene->GetPostProcessPass()->Render(scene, camera);

	// DEBUG : ImGUI 컨텍스트
	RenderImGUIContext(scene);
}

/*============================//
//   debug ui context method  //
//============================*/
#pragma region DEBUG_IMGUI_CONTEXT
void DevRenderer::RenderImGUIContext(Scene* scene)
{
    // 1. 디버깅에 필요한 요소들
    auto mainLight = static_cast<SpotLight*>(scene->GetMainLight());
    auto postProcess = scene->GetPostProcessPass();

    // 2. ImGui 렌더링 (Context가 담당?)
    IMGUI.BeginFrame();

    // imgui context #1 : 카메라
    if (IMGUI.Begin("Camera Parameters"))
    {
        ImGui::Text("TODO : ClearColor or Skybox etc.");

        // TODO : 이후에는 스카이박스, 단색의 하늘을 보여주는 로직
        // 그리고 깊이, 노멀, 위치, ambient 맵도 선택하여 볼 수 있도록 설정할 수
        // 있도록 수정 필요
        glm::vec4 clearCol = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(clearCol)))
            glClearColor(clearCol.r, clearCol.g, clearCol.b, clearCol.a);

    } IMGUI.End();

    // imgui context #2 : G-buffer 디버그
    if (IMGUI.Begin("G-buffer Debug"))
    {
        ImGui::Text("TODO : Position, Normal, Ambient");
    } IMGUI.End();

    // imgui context #2 : 조명
    if (IMGUI.Begin("Light Parameters"))
    {
        if (mainLight)
        {
            auto& lightTransform = mainLight->GetTransform();
            glm::vec3 lightPos = lightTransform.GetPosition();
            if (ImGui::DragFloat3("position", glm::value_ptr(lightPos), 0.01f))
                lightTransform.SetPosition(lightPos);

            glm::vec3 currentRotation = lightTransform.GetRotationEuler();
            bool rotationChanged = ImGui::DragFloat3("rotation", glm::value_ptr(currentRotation), 0.5f);
            if (rotationChanged) lightTransform.SetRotation(currentRotation);

            glm::vec2 lightCutoff = mainLight->GetCutoff();
            if (ImGui::DragFloat("cutoff", glm::value_ptr(lightCutoff), 0.1))
                mainLight->SetCutoff(lightCutoff);

            float lightDist = mainLight->GetDistance();
            if (ImGui::DragFloat("distance", &lightDist, 0.01))
                mainLight->SetDistance(lightDist);

            glm::vec3 ambient = mainLight->GetAmbient();
            if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient)))
                mainLight->SetAmbient(ambient);

            glm::vec3 diffuse = mainLight->GetDiffuse();
            if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse)))
                mainLight->SetDiffuse(diffuse);

            glm::vec3 specular = mainLight->GetSpecular();
            if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular)))
                mainLight->SetSpecular(specular);
        }
    } IMGUI.End();
    // imgui context #2 : 조명
    if (IMGUI.Begin("Post-Process"))
    {
        bool gammaChanged = ImGui::DragFloat("gamma", postProcess->GetGammaPtr(), 0.05f, 0.1f, 5.0f);
        bool exposureChanged = ImGui::DragFloat("exposure", postProcess->GetExposurePtr(), 0.05f, 0.1f, 10.0f);
    } IMGUI.End();

    IMGUI.EndFrame();
}
#pragma endregion