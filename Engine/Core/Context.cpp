#include "EnginePch.h"
#include "Context.h"

Context::~Context() = default;

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init()) return nullptr;
    return std::move(context);
}

void Context::Update()
{
    // 활성화된 씬을 업데이트한다.
    auto activeScene = SCENE.GetActiveScene();
    if (!activeScene) return;
    activeScene->Update();
}

void Context::Render()
{
    // 활성화된 씬이 있으니, 이 씬에 대한 렌더러를 통해 렌더링 한다.
    Scene* activeScene = SCENE.GetActiveScene();
    Renderer* activeRenderer = SCENE.GetActiveRenderer();
    if (!activeScene || !activeRenderer) return;
    activeRenderer->Render(activeScene);
}

bool Context::Init()
{
    // TODO : 첫 로드 씬은 무엇으로 할 지는 외부에서 정할 수 있도록 수정
    SCENE.LoadScene("DevScene");
    return true;
}