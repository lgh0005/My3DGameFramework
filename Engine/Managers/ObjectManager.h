#pragma once
#include "Object/Component.h"
#include "Object/GameObject.h"
#include "Components/Script.h"

class ObjectManager
{
	DECLARE_SINGLE(ObjectManager)
	DECLARE_NONINSTANTIABLE(ObjectManager)

public:
	using ComponentVector = std::vector<Component*>;
	using GOSlot = struct GameObjectSlot
	{
		GameObject* object	   { nullptr };
		uint32		generation { 1 };
		uint32		nextFree   { 0xFFFFFFFF };
	};
	static constexpr usize MAX_ENGINE_COMPONENTS = (usize)ComponentType::MAX;
	static constexpr usize INVALID_INDEX = static_cast<usize>(-1);
	static constexpr usize INVALID_ID	 = static_cast<usize>(-1);

public:
	void Init();
	void Clear();

/*==========================================//
//   GameObject instance managing methods   //
//==========================================*/
public:
	InstanceID RegisterGameObject(GameObject* obj);
	void UnregisterGameObject(InstanceID id);
	GameObject* GetGameObject(InstanceID id);
	bool IsGameObjectAlive(InstanceID id);

private:
	uint32 GetGameObjectIndex(InstanceID id);
	uint32 GetGameObjectGeneration(InstanceID id);
	InstanceID MakeGameObjectID(uint32 index, uint32 gen);

	std::vector<GOSlot> m_gameObjectSlots;
	uint32 m_goSlotfreeHead{ 0xFFFFFFFF };

/*=========================================//
//   Component instance managing methods   //
//=========================================*/
public:
	void RegisterComponent(Component* comp);
	void UnregisterComponent(Component* comp);
	template<typename T> const ComponentVector* GetComponents();

private:
	std::array<ComponentVector, MAX_ENGINE_COMPONENTS> m_componentCache;
	std::vector<ComponentVector> m_userScriptComponentCache;
};

/*==================================//
//  ObjectManager template inlines  //
//==================================*/
#pragma region OBJECT_MANAGER_TEMPLATE_INLINES
template<typename T>
inline auto ObjectManager::GetComponents() -> const ComponentVector*
{
	// 1. 스크립트인 경우
	if constexpr (std::is_base_of<Script, T>::value)
	{
		ScriptID id = ScriptRegistry::GetID<T>();

		if (id >= m_userScriptComponentCache.size())
			return nullptr;

		// 스크립트 반환
		return &m_userScriptComponentCache[id];
	}

	// 2. 그 외의 일반적인 컴포넌트인 경우
	else
	{
		usize index = static_cast<usize>(T::s_ComponentType);
		return &m_componentCache[index];
	}
}
#pragma endregion