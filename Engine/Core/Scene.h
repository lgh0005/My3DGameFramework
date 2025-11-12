#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(Camera)
CLASS_PTR(Animator)
#pragma endregion

CLASS_PTR(Scene)
class Scene
{
public:
	static SceneUPtr Create();

	void AddGameObject(GameObjectUPtr gameObject);
	void Update();

	const std::vector<MeshRenderer*>& GetRenderables() const { return m_meshes; }
	const std::vector<Light*>& GetLights() const { return m_lights; }
	const std::vector<Camera*>& GetAllCameras() const { return m_cameras; }

	Camera* GetActiveCamera() const { return m_activeCamera; }
	void SetActiveCamera(Camera* camera) { m_activeCamera = camera; }

	// TODO: 
	// - GameObject 파괴를 위한 FlushDestroyQueue 필요
	// - GameObject를 이름 등으로 찾는 FindGameObject 필요

protected:
	Scene() = default;

	// 모든 게임 오브젝트를 소유
	std::vector<GameObjectUPtr> m_gameObjects;

	// 컴포넌트 업데이트를 위한 참조 포인터
	std::vector<MeshRenderer*> m_meshes;
	std::vector<Light*> m_lights;
	std::vector<Camera*> m_cameras;
	std::vector<Animator*> m_animators;

	Camera* m_activeCamera	{ nullptr };
};