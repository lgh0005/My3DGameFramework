#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
#pragma endregion

CLASS_PTR(GameObjectRegistry)
class GameObjectRegistry
{
public:
	~GameObjectRegistry();
	static GameObjectRegistryUPtr Create();
	void AddGameObject(GameObjectUPtr go);
	void DestroyGameObject(GameObject* go);

	void FlushCreateQueue();
	void FlushDestroyQueue();
	void PushToDestroyQueue(GameObject* go);

	const std::vector<GameObjectUPtr>& GetGameObjects() const;
	const std::vector<GameObjectUPtr>& GetPendingCreateQueue() const;
	const std::vector<GameObject*>& GetPendingDestroyQueue() const;

private:
	GameObjectRegistry();
	void Init();

	std::vector<GameObjectUPtr> m_gameObjects;
	std::vector<GameObjectUPtr> m_pendingCreateQueue;
	std::vector<GameObject*>	m_pendingDestroyQueue;
};