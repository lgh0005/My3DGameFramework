#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(SkyLight)
CLASS_PTR(CubeTexture)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(RenderContext)
class RenderContext
{
	using MeshVectorRawPtr = const std::vector<MeshRenderer*>*;
	using LightVectorRawPtr = const std::vector<Light*>*;

public:
	virtual ~RenderContext();
	void Reset(Scene* scene, Camera* camera);
	void AddStaticMeshRenderer(MeshRenderer* renderer);
	void AddSkinnedMeshRenderer(MeshRenderer* renderer);
	void AddLight(Light* light);

/*====================================//
//   default render context getters   //
//====================================*/
public:
	Scene* GetScene() const;
	Camera* GetCamera() const;
	SkyLight* GetSkyLight() const;
	const std::vector<MeshRenderer*>& GetStaticMeshRenderers() const;
	const std::vector<MeshRenderer*>& GetSkinnedMeshRenderers() const;
	const std::vector<Light*>& GetLights() const;

/*====================================//
//   default render context members   //
//====================================*/
protected:

	// 씬과 카메라
	Scene* m_currentScene{ nullptr };
	Camera* m_currentCamera{ nullptr };

	// 하늘
	SkyLight* m_skyLight{ nullptr };

	// Source (Scene 원본 참조)
	MeshVectorRawPtr		   m_staticMeshRenderers;
	MeshVectorRawPtr		   m_skinnedMeshRenderers;
	LightVectorRawPtr		   m_lights;

	// Result (실제 렌더링 목록)
	std::vector<MeshRenderer*> m_culledStaticMeshRenderers;
	std::vector<MeshRenderer*> m_culledSkinnedMeshRenderers;
	std::vector<Light*>        m_culledLights;

/*======================================================//
//   common resource methods for common render passes   //
//======================================================*/
public:
	void SetShadowMap(int32 index, Texture* texture);
	Texture* GetShadowMap(int32 index) const;
	void SetSkyboxTexture(CubeTexture* texture);
	CubeTexture* GetSkyboxTexture() const;

/*===============================================//
//   common resources for common render passes   //
//===============================================*/
protected: 
	Texture* m_shadowMaps[MAX_SHADOW_CASTER] = { nullptr };
	CubeTexture* m_skyboxTexture{ nullptr };
};