#pragma once
#include <vector>

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
#pragma endregion

CLASS_PTR(RenderContext)
class RenderContext
{
public:
	virtual ~RenderContext() = default;

	void Reset(Scene* scene, Camera* camera, int32 width, int32 height);
	void AddStaticMeshRenderer(MeshRenderer* renderer);
	void AddSkinnedMeshRenderer(MeshRenderer* renderer);
	void AddLight(Light* light);

public:
	Scene* GetScene() const { return m_currentScene; }
	Camera* GetCamera() const { return m_currentCamera; }
	int32 GetWidth() const { return m_currentWidth; }
	int32 GetHeight() const { return m_currentHeight; }

	const std::vector<MeshRenderer*>& GetStaticMeshRenderers() const { return m_staticMeshRenderers; }
	const std::vector<MeshRenderer*>& GetSkinnedMeshRenderers() const { return m_skinnedMeshRenderers; }
	const std::vector<Light*>& GetLights() const { return m_lights; }

protected: 
	Scene*  m_currentScene	   { nullptr };
	Camera* m_currentCamera	   { nullptr };
	int32   m_currentWidth     { 0 };
	int32   m_currentHeight	   { 0 };

	std::vector<MeshRenderer*> m_staticMeshRenderers;
	std::vector<MeshRenderer*> m_skinnedMeshRenderers;
	std::vector<Light*>        m_lights;
};