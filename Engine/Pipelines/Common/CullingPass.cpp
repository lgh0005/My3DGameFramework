#include "EnginePch.h"
#include "CullingPass.h"
#include "Pipelines/SRP/StandardRenderContext.h"
#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Components/MeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Components/Camera.h"

CullingPass::CullingPass() = default;
CullingPass::~CullingPass() = default;

CullingPassUPtr CullingPass::Create()
{
    auto pass = CullingPassUPtr(new CullingPass());
    if (!pass->Init()) return nullptr;
    return std::move(pass);
}

bool CullingPass::Init()
{
    m_frustum = Frustum::Create();
    if (!m_frustum) return false;
    return true;
}

void CullingPass::Render(RenderContext* context)
{
    // Context는 StandardRenderContext로 다운캐스팅 가능하다고 가정
    StandardRenderContext* stdContext = (StandardRenderContext*)context;
    Scene* scene = stdContext->GetScene();
    Camera* camera = stdContext->GetCamera();

    // 1. Frustum 업데이트 (카메라의 View-Projection 행렬 사용)
    m_frustum->Update(camera->GetViewProjectionMatrix());

    // 2. Static Mesh Renderer 컬링
    CullStaticMeshRenderers(scene, stdContext);

    // 3. Skinned Mesh Renderer 컬링
    CullSkinnedMeshRenderers(scene, stdContext);

    // 4. Mesh Outline 컬링
    CullMeshOutlines(scene, stdContext);

    // 5. Light는 컬링하지 않고 모두 포함 (조명 범위는 Geometry Pass 결과에 의존)
    CullSceneLights(scene, stdContext);
}

/*=====================//
//   culling methods   //
//=====================*/
void CullingPass::CullStaticMeshRenderers(Scene* scene, StandardRenderContext* context)
{
    // TODO : 최적화가 가능한 요소로 옥트트리와 같은 공간 분할로
    // 절두체 범위에 들어가는 대상들을 빠르게 탐색 가능 (O(logn))
    for (auto* renderer : scene->GetStaticMeshRenderers())
    {
        // O(N) 선형 검사: CheckBounds가 성공하면 (화면에 보이거나 걸쳐 있으면)
        if (m_frustum->CheckBounds(renderer->GetWorldBounds()))
            context->AddStaticMeshRenderer(renderer);
    }
}

void CullingPass::CullSkinnedMeshRenderers(Scene* scene, StandardRenderContext* context)
{
    // 3. Skinned Mesh Renderer 컬링
    for (auto* renderer : scene->GetSkinnedMeshRenderers())
    {
        // Skinned Mesh는 GetWorldBounds() 내부에 Animator/Transform 로직이 포함되어 있습니다.
        if (m_frustum->CheckBounds(renderer->GetWorldBounds()))
            context->AddSkinnedMeshRenderer(renderer);
    }
}

void CullingPass::CullMeshOutlines(Scene* scene, StandardRenderContext* context)
{
    // Scene이 가지고 있는 전체 아웃라인 리스트 순회
    const auto& outlines = scene->GetMeshOutlines();
    for (auto* outline : outlines)
    {
        // 1. 소유자(GameObject) 확인
        auto owner = outline->GetOwner();
        if (!owner) continue;

        // 2. 형제 컴포넌트인 MeshRenderer 찾기
        auto renderer = owner->GetComponent<MeshRenderer>();

        // 렌더러가 없으면 그릴 형체도 없으므로 스킵
        if (!renderer) continue;

        // 3. 렌더러의 바운딩 박스를 이용해 절두체 검사
        // TODO :엄밀히 따지면 아웃라인 두께만큼 Bounds를 확장해야 할 수 있음.
        if (m_frustum->CheckBounds(renderer->GetWorldBounds()))
            context->AddMeshOutline(outline);
    }
}

void CullingPass::CullSceneLights(Scene* scene, StandardRenderContext* context)
{
    // LightSource는 LightPass가 사용하므로 그대로 복사합니다.
    // TODO : 이후에 Light Volume에 의한 최적화를 위해 일단 그대로 둔다.
    const auto& lightSource = scene->GetLights();
    for (auto* light : lightSource) context->AddLight(light);
}

