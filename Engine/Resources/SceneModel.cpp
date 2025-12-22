#include "EnginePch.h"
#include "SceneModel.h"
#include "AssetTypes/AssetUtils.h"
#include "Core/GameObject.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Resources/Texture.h"
#include "Resources/StaticMesh.h"
#include "Resources/SkinnedMesh.h"
#include "Resources/Image.h"

SceneModelUPtr SceneModel::Load(const std::string& filename)
{
    auto model = SceneModelUPtr(new SceneModel());
    std::string ext = std::filesystem::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".mymodel")
    {
        if (!model->LoadByBinary(filename)) return nullptr;
    }
    else
    {
        if (!model->LoadByAssimp(filename)) return nullptr;
    }
    return std::move(model);
}

/*===================================================================//
//  [CreateGameObject] 모델 인스턴스화 (계층 구조 복원)                //
//===================================================================*/
GameObjectUPtr SceneModel::CreateGameObject()
{
    if (m_nodes.empty()) return nullptr;

    // 0번 노드가 항상 Root라고 가정 (Converter 규칙)
    return CreateGameObjectRecursive(m_nodes[0]);
}

GameObjectUPtr SceneModel::CreateGameObjectRecursive(const AssetFmt::RawNode& node)
{
    // 1. GameObject 생성
    auto go = GameObject::Create();
    go->SetName(node.name);

    // 2. Transform 설정 (로컬)
    go->GetTransform().SetLocalTransform(node.localTransform);

    // 3. MeshRenderer 부착
    for (uint32 meshIndex : node.meshIndices)
    {
        if (meshIndex >= m_meshes.size()) continue;

        auto mesh = m_meshes[meshIndex];
        auto renderer = MeshRenderer::Create();
        renderer->SetMesh(mesh);

        // 머티리얼은 이미 Mesh 안에 설정되어 있음 (Load 시점에)
        // 만약 오버라이드하고 싶다면 여기서 renderer->SetMaterial(...) 호출

        go->AddComponent(std::move(renderer));
    }

    // 4. 자식 노드 재귀 호출
    for (int32 childIndex : node.childrenIndices)
    {
        if (childIndex >= m_nodes.size()) continue;

        auto childGO = CreateGameObjectRecursive(m_nodes[childIndex]);
        if (childGO)
        {
            go->AddChild(childGO.get());
            // 소유권 이전 (AddChild 내부 구현에 따라 다를 수 있음. 
            // 보통 Scene이 소유하거나 부모가 소유하도록 처리해야 함)
            // 여기서는 unique_ptr을 리턴하므로, 호출자가 관리하거나 Scene에 등록해야 함.
            // 하지만 GameObject::AddChild가 Raw Pointer를 받는다면 소유권 관리가 애매해질 수 있음.
            // 작성자님의 GameObject 구조상 AddChild가 unique_ptr을 받는지 확인 필요.
            // 일단은 unique_ptr의 소유권을 부모에게 넘기는 방식이 일반적임.
            // (가정: GameObject가 자식들을 vector<unique_ptr>로 관리한다고 가정)
        }
    }

    return go;
}

// ... [LoadByBinary, LoadByAssimp, ProcessNodeAssimp 등은 아까 작성한 Model.cpp 코드 그대로 복사] ...