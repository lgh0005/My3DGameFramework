#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(StaticMeshRenderer)
CLASS_PTR(SkinnedMeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(MeshOutline)
CLASS_PTR(SkyLight)
CLASS_PTR(CubeTexture)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(RenderContext)
class RenderContext
{
	using StaticMeshVectorRawPtr = const std::vector<StaticMeshRenderer*>*;
	using SkinnedMeshVectorRawPtr = const std::vector<SkinnedMeshRenderer*>*;
	using LightVectorRawPtr = const std::vector<Light*>*;

public:
	virtual ~RenderContext();
	void Reset(Scene* scene, Camera* camera);
	void AddMeshOutline(MeshOutline* outline);
	void AddStaticMeshRenderer(StaticMeshRenderer* renderer);
	void AddSkinnedMeshRenderer(SkinnedMeshRenderer* renderer);
	void AddLight(Light* light);

/*====================================//
//   default render context getters   //
//====================================*/
public:
	Scene* GetScene() const;
	Camera* GetCamera() const;
	SkyLight* GetSkyLight() const;
	const std::vector<StaticMeshRenderer*>& GetStaticMeshRenderers() const;
	const std::vector<SkinnedMeshRenderer*>& GetSkinnedMeshRenderers() const;
	const std::vector<MeshOutline*>& GetMeshOutlines() const;
	const std::vector<Light*>& GetLights() const;

/*====================================//
//   default render context members   //
//====================================*/
protected:
	RenderContext();

	// 씬과 카메라
	Scene* m_currentScene{ nullptr };
	Camera* m_currentCamera{ nullptr };

	// 하늘
	SkyLight* m_skyLight{ nullptr };

	// Source (Scene 원본 참조)
	StaticMeshVectorRawPtr		   m_staticMeshRenderers;
	SkinnedMeshVectorRawPtr		   m_skinnedMeshRenderers;
	LightVectorRawPtr		   m_lights;

	// Result (실제 렌더링 목록)
	std::vector<MeshOutline*>  m_culledMeshOutlines;
	std::vector<StaticMeshRenderer*> m_culledStaticMeshRenderers;
	std::vector<SkinnedMeshRenderer*> m_culledSkinnedMeshRenderers;
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