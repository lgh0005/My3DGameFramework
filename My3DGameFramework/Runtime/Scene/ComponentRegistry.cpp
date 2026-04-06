#include "EnginePch.h"
#include "ComponentRegistry.h"

#include "Object/GameObject.h"
#include "Graphics/Rendering/RenderPass.h"
#include "Components/SkyLight.h"
#include "Components/Light.h"
#include "Components/PointLight.h"
#include "Components/DirectionalLight.h"
#include "Components/SpotLight.h"
#include "Components/MeshRenderer.h"
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

ComponentRegistry::ComponentRegistry() = default;
ComponentRegistry::~ComponentRegistry()
{
	for (auto& vec : m_componentCache) vec.clear();
	m_customPasses.clear();
}

ComponentRegistryUPtr ComponentRegistry::Create()
{
	auto registry = ComponentRegistryUPtr(new ComponentRegistry());
	registry->Init();
	return std::move(registry);
}

void ComponentRegistry::Init()
{
	for (auto& vec : m_componentCache) vec.reserve(32);
}

void ComponentRegistry::RegisterComponent(Component* component)
{
	if (!component) return;
	ComponentType type = component->GetComponentType();

	// 1. 상속 구조에 있는 컴포넌트 이중 등록
	switch (type)
	{
		// [Light 계열]
		case ComponentType::DirectionalLight:
		case ComponentType::PointLight:
		case ComponentType::SpotLight:
		{
			usize baseIndex = (usize)ComponentType::Light;
			m_componentCache[baseIndex].push_back(component);
			break;
		}

		// [MeshRenderer 계열]
		case ComponentType::StaticMeshRenderer:
		case ComponentType::SkinnedMeshRenderer:
		case ComponentType::InstancedMeshRenderer:
		{
			// Forward 스테이지 메쉬 렌더러들은 제외
			auto mr = static_cast<MeshRenderer*>(component);
			if (mr->GetRenderStage() == RenderStage::Forward) return;

			usize baseIndex = (usize)ComponentType::MeshRenderer;
			m_componentCache[baseIndex].push_back(component);
			break;
		}

		// [Collider 계열]
		case ComponentType::BoxCollider:
		case ComponentType::SphereCollider:
		case ComponentType::CapsuleCollider:
		{
			usize baseIndex = (usize)ComponentType::Collider;
			m_componentCache[baseIndex].push_back(component);
			break;
		}
	}

	// 2. 자기 자신의 구체적 타입(Leaf Class) 위치에 저장
	usize index = (usize)type;
	auto& vec = m_componentCache[index];
	vec.push_back(component);
	component->SetRegistryIndex(vec.size() - 1);
}

void ComponentRegistry::UnregisterComponent(Component* component)
{
	if (!component) return;
	ComponentType type = component->GetComponentType();

	// 1. 이중 해제 로직
	switch (type)
	{
		// [Light 계열]
		case ComponentType::DirectionalLight:
		case ComponentType::PointLight:
		case ComponentType::SpotLight:
		{
			RemoveComponentFromBaseList(ComponentType::Light, component);
			break;
		}

		// [MeshRenderer 계열]
		case ComponentType::StaticMeshRenderer:
		case ComponentType::SkinnedMeshRenderer:
		case ComponentType::InstancedMeshRenderer:
		{
			RemoveComponentFromBaseList(ComponentType::MeshRenderer, component);
			break;
		}

		// [Collider 계열]
		case ComponentType::BoxCollider:
		case ComponentType::SphereCollider:
		case ComponentType::CapsuleCollider:
		{
			RemoveComponentFromBaseList(ComponentType::Collider, component);
			break;
		}
	}

	// 2. 자기 자신의 구체적 타입 위치에서 삭제
	usize index = (usize)type;
	if (index >= m_componentCache.size()) return;
	auto& vec = m_componentCache[index];

	usize regIdx = component->GetRegistryIndex();
	if (regIdx >= vec.size() || vec[regIdx] != component) return;

	Component* lastComp = vec.back();
	vec[regIdx] = lastComp;
	lastComp->SetRegistryIndex(regIdx);
	vec.pop_back();

	component->SetRegistryIndex(Component::INVALID_REGISTRY_IDX);
}

void ComponentRegistry::RemoveComponentFromBaseList(ComponentType baseType, Component* component)
{
	auto& vec = m_componentCache[(usize)baseType];
	auto it = std::find(vec.begin(), vec.end(), component);
	if (it != vec.end())
	{
		*it = vec.back();
		vec.pop_back();
	}
}

/*=======================================//
//   getters and setters of components   //
//=======================================*/
Camera* ComponentRegistry::GetCamera(uint32 idx) const
{
	const auto& cameras = m_componentCache[(usize)ComponentType::Camera];

	if (idx >= cameras.size())
	{
		LOG_ERROR("Camera index out of range: {}", idx);
		return nullptr;
	}

	return static_cast<Camera*>(cameras[idx]);
}

void ComponentRegistry::AddCustomRenderPass(const std::string& name, GeneralRenderPassUPtr pass)
{
	if (m_customPasses.find(name) != m_customPasses.end())
		LOG_WARN("Custom random pass '{}' already exists. Overwriting.", name);

	m_customPasses[name] = std::move(pass);
}

GeneralRenderPass* ComponentRegistry::GetCustomRenderPass(const std::string& name)
{
	auto it = m_customPasses.find(name);
	if (it != m_customPasses.end()) return it->second.get();
	return nullptr;
}

ComponentRegistry::CustomPassMap& ComponentRegistry::GetCustomRenderPasses()
{
	return m_customPasses;
}

SkyLight* ComponentRegistry::GetSkyLight(uint32 idx) const
{
	const auto& sky = m_componentCache[(usize)ComponentType::SkyLight];

	if (idx >= sky.size())
	{
		LOG_ERROR("Skybox index out of range: {}", idx);
		return nullptr;
	}

	return static_cast<SkyLight*>(sky[idx]);
}