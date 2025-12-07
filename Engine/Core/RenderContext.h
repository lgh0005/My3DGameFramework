#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
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

	const std::vector<MeshRenderer*>& GetStaticMeshRenderers() const { return m_culledStaticMeshRenderers; }
	const std::vector<MeshRenderer*>& GetSkinnedMeshRenderers() const { return m_culledSkinnedMeshRenderers; }
	const std::vector<Light*>& GetLights() const { return m_culledLights; }

protected: 
	Scene*  m_currentScene	   { nullptr };
	Camera* m_currentCamera	   { nullptr };

	// Source (Scene 원본 참조)
	MeshVectorRawPtr	m_staticMeshRenderers;
	MeshVectorRawPtr	m_skinnedMeshRenderers;
	LightVectorRawPtr   m_lights;

	// Result (실제 렌더링 목록)
	// TODO : Light은 그냥 씬에 있는 걸 쓰도록 수정 필요
	// 지금 당장 조명을 컬링할 생각은 없음
	std::vector<MeshRenderer*> m_culledStaticMeshRenderers;
	std::vector<MeshRenderer*> m_culledSkinnedMeshRenderers;
	std::vector<Light*>        m_culledLights;
};