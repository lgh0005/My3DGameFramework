#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
#pragma endregion

CLASS_PTR(GameObjectRegistry)
class GameObjectRegistry
{
public:
	~GameObjectRegistry();
	struct GameObjectSlot
	{
		GameObject* object{ nullptr };
		uint32      generation{ 1 };
		uint32      nextFree{ 0xFFFFFFFF };
	};
	static constexpr usize INVALID_ID = static_cast<usize>(-1);

private:
	GameObjectRegistry();
	void Init();

/*======================================//
//   default GameObject queue methods   //
//======================================*/
public:
	static GameObjectRegistryUPtr Create();
	void AddGameObject(GameObjectUPtr go);
	void DestroyGameObject(GameObject* go);
	std::string MakeUniqueObjectName(const std::string& baseName);

	void FlushCreateQueue();
	void FlushDestroyQueue();
	void PushToDestroyQueue(GameObject* go);

	const std::vector<GameObjectUPtr>& GetGameObjects() const;
	const std::vector<GameObjectUPtr>& GetPendingCreateQueue() const;
	const std::vector<GameObject*>& GetPendingDestroyQueue() const;

private:

	std::vector<GameObjectUPtr> m_gameObjects;
	std::vector<GameObjectUPtr> m_pendingCreateQueue;
	std::vector<GameObject*>	m_pendingDestroyQueue;

/*================================//
//   GameObject finding methods   //
//================================*/
public:
	GameObject* GetGameObjectByID(InstanceID id);     // O(1)
	GameObject* FindGameObjectByName(const std::string& name); // O(N)

private:
	InstanceID RegisterGameObjectID(GameObject* obj);
	void UnregisterGameObjectID(InstanceID id);

	uint32 GetIndexFromID(InstanceID id);
	uint32 GetGenerationFromID(InstanceID id);
	InstanceID MakeID(uint32 index, uint32 gen);

	std::vector<GameObjectSlot> m_gameObjectSlots;
	uint32 m_freeSlotHead{ 0xFFFFFFFF };
};