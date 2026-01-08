#include "EnginePch.h"
#include "SceneRegistry.h"

#include "GameObject/GameObject.h"
#include "Graphics/RenderPass.h"
#include "Resources/Mesh.h"
#include "Graphics/SkyLight.h"
#include "Components/Light.h"
#include "Components/PointLight.h"
#include "Components/DirectionalLight.h"
#include "Components/StaticMeshRenderer.h"
#include "Components/SkinnedMeshRenderer.h"
#include "Components/InstancedMeshRenderer.h"
#include "Components/MeshOutline.h"
#include "Components/Camera.h"
#include "Components/Animator.h"
#include "Components/Script.h"
#include "Components/Transform.h"
#include "Components/AudioSource.h"
#include "Components/AudioListener.h"
#include "Components/Collider.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/Rigidbody.h"
#include "UIs/UICanvas.h"

DECLARE_DEFAULTS_IMPL(SceneRegistry)

SceneRegistryUPtr SceneRegistry::Create()
{
	return SceneRegistryUPtr(new SceneRegistry());
}

void SceneRegistry::RegisterComponent(Component* component)
{
	if (!component) return;

	// TODO : 이후에 다른 게임 컴포넌트들도 나열해야함
	switch (component->GetComponentType())
	{
		case ComponentType::Camera:
		{
			m_cameras.push_back(static_cast<Camera*>(component));
			break;
		}
		case ComponentType::Light:
		case ComponentType::DirectionalLight:
		case ComponentType::PointLight:
		case ComponentType::SpotLight:
		{
			m_lights.push_back(static_cast<Light*>(component));
			break;
		}
		case ComponentType::Animator:
		{
			m_animators.push_back(static_cast<Animator*>(component));
			break;
		}
		case ComponentType::Script:
		{
			m_scripts.push_back(static_cast<Script*>(component));
			break;
		}
		case ComponentType::AudioSource:
		{
			m_audioSources.push_back(static_cast<AudioSource*>(component));
			break;
		}
		case ComponentType::AudioListener:
		{
			m_audioListeners.push_back(static_cast<AudioListener*>(component));
			break;
		}
		case ComponentType::StaticMeshRenderer:
		{
			auto mr = static_cast<StaticMeshRenderer*>(component);
			if (mr->GetRenderStage() == RenderStage::Forward) return;
			m_staticMeshRenderers.push_back(mr);
			break;
		}
		case ComponentType::SkinnedMeshRenderer:
		{
			auto mr = static_cast<SkinnedMeshRenderer*>(component);
			if (mr->GetRenderStage() == RenderStage::Forward) return;
			m_skinnedMeshRenderers.push_back(mr);
			break;
		}
		case ComponentType::InstancedMeshRenderer:
		{
			auto mr = static_cast<InstancedMeshRenderer*>(component);
			if (mr->GetRenderStage() == RenderStage::Forward) return;
			m_instancedMeshRenderers.push_back(mr);
			break;
		}
		case ComponentType::MeshOutline:
		{
			m_outlines.push_back(static_cast<MeshOutline*>(component));
			break;
		}
		case ComponentType::UICanvas:
		{
			m_uiCanvases.push_back(static_cast<UICanvas*>(component));
			break;
		}
		case ComponentType::Collider:
		case ComponentType::BoxCollider:
		case ComponentType::SphereCollider:
		{
			m_colliders.push_back(static_cast<Collider*>(component));
			break;
		}

		case ComponentType::Rigidbody:
		{
			m_rigidBodies.push_back(static_cast<Rigidbody*>(component));
			break;
		}
	}
}

void SceneRegistry::UnregisterComponent(Component* component)
{
	if (!component) return;

	// 등록과 정확히 대칭되는 해제 로직
	switch (component->GetComponentType())
	{
		case ComponentType::Camera:
			RemoveFromVector(m_cameras, component); break;
		case ComponentType::Light:
		case ComponentType::DirectionalLight:
		case ComponentType::PointLight:
		case ComponentType::SpotLight:
			RemoveFromVector(m_lights, component); break;
		case ComponentType::Animator:
			RemoveFromVector(m_animators, component); break;
		case ComponentType::Script:
			RemoveFromVector(m_scripts, component); break;
		case ComponentType::AudioSource:
			RemoveFromVector(m_audioSources, component); break;
		case ComponentType::AudioListener:
			RemoveFromVector(m_audioListeners, component); break;
		case ComponentType::StaticMeshRenderer:
			RemoveFromVector(m_staticMeshRenderers, component); break;
		case ComponentType::SkinnedMeshRenderer:
			RemoveFromVector(m_skinnedMeshRenderers, component); break;
		case ComponentType::InstancedMeshRenderer:
			RemoveFromVector(m_instancedMeshRenderers, component); break;
		case ComponentType::MeshOutline:
			RemoveFromVector(m_outlines, component); break;
		case ComponentType::UICanvas:
			RemoveFromVector(m_uiCanvases, component); break;
		case ComponentType::Collider:
		case ComponentType::BoxCollider:
		case ComponentType::SphereCollider:
			RemoveFromVector(m_colliders, component); break;
		case ComponentType::Rigidbody:
			RemoveFromVector(m_rigidBodies, component); break;
	}
}

/*=======================================//
//   getters and setters of components   //
//=======================================*/
Camera* SceneRegistry::GetCamera(uint32 idx) const
{
	if (idx >= m_cameras.size())
	{
		LOG_ERROR("Camera index out of range: {}", idx);
		return nullptr;
	}
	return m_cameras[idx];
}

void SceneRegistry::AddCustomRenderPass(const std::string& name, GeneralRenderPassUPtr pass)
{
	if (m_customPasses.find(name) != m_customPasses.end())
		LOG_WARN("Custom random pass '{}' already exists. Overwriting.", name);

	m_customPasses[name] = std::move(pass);
}

GeneralRenderPass* SceneRegistry::GetCustomRenderPass(const std::string& name)
{
	auto it = m_customPasses.find(name);
	if (it != m_customPasses.end()) return it->second.get();
	return nullptr;
}

void SceneRegistry::SetSkyLight(SkyLightUPtr skyLight)
{
	m_sky = std::move(skyLight);
}

SkyLight* SceneRegistry::GetSkyLight() const
{
	return m_sky.get();
}