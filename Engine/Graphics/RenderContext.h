#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(ComponentRegistry)
CLASS_PTR(Component)
CLASS_PTR(Camera)
CLASS_PTR(StaticMeshRenderer)
CLASS_PTR(SkinnedMeshRenderer)
CLASS_PTR(Light)
CLASS_PTR(MeshOutline)
CLASS_PTR(SkyLight)
CLASS_PTR(CubeTexture)
CLASS_PTR(Texture)
CLASS_PTR(GBufferFramebuffer) // 추가
CLASS_PTR(Framebuffer)        // 추가
#pragma endregion

enum class RenderSlot : uint8
{
	GPosition, GNormal, GAlbedo, GEmission, GVelocity, // Geometry Pass Output
	SSAO,                                              // SSAO Pass Output
	MotionBlur,                                        // Motion Blur / PostProcess Input
	MAX
};

CLASS_PTR(RenderContext)
class RenderContext
{
	using ComponentVectorRawPtr = const std::vector<Component*>*;

/*====================================//
//   default render context methods   //
//====================================*/
public:
	RenderContext();
	virtual ~RenderContext();
	void Reset(Scene* scene, Camera* camera);
	void AddMeshOutline(MeshOutline* outline);
	void AddStaticMeshRenderer(StaticMeshRenderer* renderer);
	void AddSkinnedMeshRenderer(SkinnedMeshRenderer* renderer);
	void AddLight(Light* light);

/*====================================//
//    Resource & Target Management    //
//====================================*/
public:
	void SetGBuffer(GBufferFramebuffer* gBuffer);
	void SetTexture(RenderSlot slot, Texture* texture) { m_textures[(uint8)slot] = texture; }
	Texture* GetTexture(RenderSlot slot) const { return m_textures[(uint8)slot]; }
	void SetTargetFramebuffer(Framebuffer* fbo) { m_targetFramebuffer = fbo; }
	Framebuffer* GetTargetFramebuffer() const { return m_targetFramebuffer; }
	void BindTargetFramebuffer() const;

/*====================================//
//   default render context getters   //
//====================================*/
public:
	ComponentRegistry* GetSceneRegistry() const;
	Camera* GetCamera() const;
	SkyLight* GetSkyLight() const;

	const std::vector<StaticMeshRenderer*>& GetStaticMeshRenderers() const;
	const std::vector<SkinnedMeshRenderer*>& GetSkinnedMeshRenderers() const;
	const std::vector<MeshOutline*>& GetMeshOutlines() const;
	const std::vector<Light*>& GetLights() const;

	const ComponentVectorRawPtr GetSourceStaticMeshes() const;
	const ComponentVectorRawPtr GetSourceSkinnedMeshes() const;
	const ComponentVectorRawPtr GetSourceMeshOutlines() const;
	const ComponentVectorRawPtr GetSourceLights() const;

/*====================================//
//   default render context members   //
//====================================*/
protected:

	// 씬과 카메라
	ComponentRegistry* m_componentRegistry			      { nullptr };
	Camera* m_currentCamera								  { nullptr };

	// 하늘
	SkyLight* m_skyLight								  { nullptr };

	// Source (Scene 원본 참조)
	ComponentVectorRawPtr		m_staticMeshRenderersSrc  { nullptr };
	ComponentVectorRawPtr		m_skinnedMeshRenderersSrc { nullptr };
	ComponentVectorRawPtr		m_meshOutlinesSrc		  { nullptr };
	ComponentVectorRawPtr		m_lightsSrc				  { nullptr };

	// Result (실제 렌더링 목록)
	std::vector<StaticMeshRenderer*> m_culledStaticMeshRenderers;
	std::vector<SkinnedMeshRenderer*> m_culledSkinnedMeshRenderers;
	std::vector<MeshOutline*>  m_culledMeshOutlines;
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

	Framebuffer* m_targetFramebuffer{ nullptr };
	std::array<Texture*, (size_t)RenderSlot::MAX> m_textures;
};