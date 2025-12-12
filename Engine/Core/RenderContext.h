#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(SkyLight)
#pragma endregion

CLASS_PTR(RenderContext)
class RenderContext
{
	using MeshVectorRawPtr = const std::vector<MeshRenderer*>*;
	using LightVectorRawPtr = const std::vector<Light*>*;

public:
	virtual ~RenderContext();
	void Reset(Scene* scene, Camera* camera);

public:
	Scene* GetScene() const { return m_currentScene; }
	Camera* GetCamera() const { return m_currentCamera; }

	SkyLight* GetSkyLight() const { return m_skyLight; }

	const std::vector<MeshRenderer*>& GetStaticMeshRenderers() const { return m_culledStaticMeshRenderers; }
	const std::vector<MeshRenderer*>& GetSkinnedMeshRenderers() const { return m_culledSkinnedMeshRenderers; }
	const std::vector<Light*>& GetLights() const { return m_culledLights; }

	void AddStaticMeshRenderer(MeshRenderer* renderer);
	void AddSkinnedMeshRenderer(MeshRenderer* renderer);
	void AddLight(Light* light);

protected: 

	// 씬과 카메라
	Scene*  m_currentScene	   { nullptr };
	Camera* m_currentCamera	   { nullptr };

	// 하늘
	SkyLight* m_skyLight	   { nullptr };

	// Source (Scene 원본 참조)
	MeshVectorRawPtr		   m_staticMeshRenderers;
	MeshVectorRawPtr		   m_skinnedMeshRenderers;
	LightVectorRawPtr		   m_lights;

	// Result (실제 렌더링 목록)
	std::vector<MeshRenderer*> m_culledStaticMeshRenderers;
	std::vector<MeshRenderer*> m_culledSkinnedMeshRenderers;
	std::vector<Light*>        m_culledLights;

	// TODO :
	// 공통으로 빼둔 렌더 패스들이 요구하는 자원들을, 
	// RenderContext에다가 이주를 시킬 필요가 있다.
};