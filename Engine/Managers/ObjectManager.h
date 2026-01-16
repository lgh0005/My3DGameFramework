#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
CLASS_PTR(Script)
#pragma endregion

class ObjectManager
{
	DECLARE_SINGLE(ObjectManager)

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
	~ObjectManager();
	void Init();
	void Clear();

private:
	ObjectManager();

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

/*===================================//
//   runtime GameObject management   //
//===================================*/
public:
	void AddGameObject(GameObjectUPtr go);
	void DestroyGameObject(GameObject* go);
	void ProcessPendingCreates(bool isSceneAwake, bool isSceneRunning);
	void ProcessPendingDestroys();

	const std::vector<GameObjectUPtr>& GetGameObjects() const;
	const std::vector<GameObjectUPtr>& GetPendingCreateQueue() const;
	const std::vector<GameObject*>& GetPendingDestroyQueue() const;

private:
	void FlushCreateQueue();
	void FlushDestroyQueue();

	std::vector<GameObjectUPtr> m_gameObjects;
	std::vector<GameObjectUPtr> m_pendingCreateQueue;
	std::vector<GameObject*>	m_pendingDestroyQueue;
};

#include "Managers/ObjectManager.inl"