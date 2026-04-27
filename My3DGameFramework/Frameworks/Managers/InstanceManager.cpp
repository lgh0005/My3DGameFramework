#include "FrameworkPch.h"
#include "InstanceManager.h"
#include "Managers/EntityManager.h"
#include "Rendering/RenderContext.h"
#include "Components/Transform.h"
#include "Components/MeshRenderers/MeshRenderer.h"
//#include "Components/MeshRenderers/SkinnedMeshRenderer.h"


namespace MGF3D
{
	InstanceManager::InstanceManager() = default;
	InstanceManager::~InstanceManager() = default;

    void InstanceManager::Extract(RenderContext* context)
    {
        // 1. 메쉬 렌더러 추출
        ExtractStaticMeshes(context);
        ExtractSkinnedMeshes(context);
    }

    void InstanceManager::ExtractStaticMeshes(RenderContext* context)
    {
        auto* registry = MGF_ENTITY.GetComponentRegistry<MeshRenderer>();
        if (!registry) return;

        const auto& renderers = registry->GetComponents();
        for (const auto& renderer : renderers)
        {
            if (renderer.GetType()->selfIndex != MeshRenderer::s_typeIndex)
                continue;

            auto* transform = MGF_ENTITY.GetComponent<Transform>(renderer.GetOwnerID());
            if (!transform) continue;

            Mesh* mesh = renderer.GetMesh();
            Material* material = renderer.GetMaterial();
            if (!mesh || !material) continue;

            StaticInstanceData data;
            data.worldMatrix = transform->GetWorldMatrix();

            context->GetStaticQueue().Submit(mesh, material, data);
        }
    }

    void InstanceManager::ExtractSkinnedMeshes(RenderContext* context)
    {
  //      auto* registry = MGF_ENTITY.GetComponentRegistry<SkinnedMeshRenderer>();
  //      if (!registry) return;

  //      const auto& components = registry->GetComponents();
		//for (const auto& renderer : components)
		//{
		//	if (renderer.GetType()->selfIndex != SkinnedMeshRenderer::s_typeIndex)
		//		continue;

		//	auto* transform = MGF_ENTITY.GetComponent<Transform>(renderer.GetOwnerID());
		//	if (!transform) continue;

		//	Mesh* mesh = renderer.GetMesh();
		//	Material* material = renderer.GetMaterial();
		//	if (!mesh || !material) continue;

		//	const Vector<mat4>* boneMatrices = renderer.GetBoneMatrices();
		//	if (!boneMatrices) continue;

		//	SkinnedInstanceData data;
		//	data.worldMatrix = transform->GetWorldMatrix();
		//	data.boneMatrices = *boneMatrices;

		//	context->GetSkinnedQueue().Submit(mesh, material, data);
		//}
    }
}