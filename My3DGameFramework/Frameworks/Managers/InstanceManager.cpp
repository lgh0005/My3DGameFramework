#include "FrameworkPch.h"
#include "InstanceManager.h"
#include "Managers/EntityManager.h"
#include "Rendering/RenderContext.h"
#include "Instancing/Meshes/StaticInstanceData.h"
#include "Components/Transform.h"
#include "Graphics/Meshes/StaticMesh.h"
#include "Graphics/Meshes/SkinnedMesh.h"
#include "Components/MeshRenderers/MeshRenderer.h"
#include "Components/MeshRenderers/SkinnedMeshRenderer.h"
#include "Components/Animation/Animator.h"

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
        for (const auto* renderer : renderers)
        {
            if (renderer->GetType()->selfIndex != MeshRenderer::s_typeIndex)
                continue;

            auto* transform = MGF_ENTITY.GetComponent<Transform>(renderer->GetOwnerID());
            if (!transform) continue;

            StaticMesh* mesh = renderer->GetMesh();
            Material* material = renderer->GetMaterial();
            if (!mesh || !material) continue;

            StaticInstanceData data;
            data.worldMatrix = transform->GetWorldMatrix();

            context->GetStaticQueue().Submit(mesh, material, data);
        }
    }

    void InstanceManager::ExtractSkinnedMeshes(RenderContext* context)
    {
        auto* registry = MGF_ENTITY.GetComponentRegistry<SkinnedMeshRenderer>();
        if (!registry) return;

        const auto& components = registry->GetComponents();
		for (const auto& renderer : components)
		{
	        if (renderer->GetType()->selfIndex != SkinnedMeshRenderer::s_typeIndex)
			   continue;

		    auto* transform = MGF_ENTITY.GetComponent<Transform>(renderer->GetOwnerID());
		    if (!transform) continue;

		    SkinnedMesh* mesh = renderer->GetSkinnedMesh();
		    Material* material = renderer->GetMaterial();
		    if (!mesh || !material) continue;

		    SkinnedInstanceData data;
		    data.worldMatrix = transform->GetWorldMatrix();
            ObjectIDHash rootID = renderer->GetRootEntityID();
            auto* animator = MGF_ENTITY.GetComponent<Animator>(rootID);
            if (animator) data.boneOffset = animator->GetBoneBufferOffset();
            else data.boneOffset = 0;

		    context->GetSkinnedQueue().Submit(mesh, material, data);
		}
    }

	void InstanceManager::ExtractOutlineMeshes(RenderContext* context)
	{
		// Outline 메쉬 추출 로직 구현
	}
}