#pragma once
#include "Instancing/RenderQueue.h"
#include "Instancing/Meshes/StaticInstanceData.h"
#include "Instancing/Meshes/SkinnedInstanceData.h"
#include "Instancing/Lights/DirectionalLightData.h"
#include "Instancing/Lights/PointLightData.h"
#include "Instancing/Lights/SpotLightData.h"
#include "Instancing/Shadows/DirectionalShadowData.h"
#include "Instancing/Shadows/PointShadowData.h"
#include "Instancing/Shadows/SpotShadowData.h"
#include "Uniforms/GlobalUniform.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Camera)
	MGF_CLASS_PTR(SkyLight)
	MGF_CLASS_PTR(GLUniformBuffer)
	MGF_CLASS_PTR(GLShaderStorageBuffer)
	MGF_CLASS_PTR(GLTexture2D)
	MGF_CLASS_PTR(GLTexture2DArray)
	MGF_CLASS_PTR(GLTextureCubeArray)
	MGF_CLASS_PTR(ScreenMesh)
	MGF_CLASS_PTR(GLFramebuffer2D)

	enum class EGBufferSlot : uint8
	{
		PositionAO = 0,      // location = 0
		NormalRoughness,     // location = 1
		AlbedoMetallic,      // location = 2
		Emission,            // location = 3
		Velocity,            // location = 4
		Max
	};

	enum class ETextureCache : uint8
	{
		SSAO,
		Bloom,
		Max
	};

	MGF_CLASS_PTR(RenderContext)
	class RenderContext
	{
	public:
		RenderContext();
		~RenderContext();

	public:
		auto& GetStaticQueue() { return m_staticQueue; }
		auto& GetSkinnedQueue() { return m_skinnedQueue; }
		void ClearQueues();

	public:
		void UpdateGlobals(const GlobalData& globalData);
		void SetCurrentCamera(const Camera* camera) { m_currentCamera = camera; }
		const Camera* GetCurrentCamera() const { return m_currentCamera; }
		void SetMainSkyLight(const SkyLight* skyLight) { m_mainSkyLight = skyLight; }
		const SkyLight* GetMainSkyLight() const { return m_mainSkyLight; }
		
		void UpdateDirectionalLights(const Vector<DirectionalLightData>& lights);
		void UpdatePointLights(const Vector<PointLightData>& lights);
		void UpdateSpotLights(const Vector<SpotLightData>& lights);

		void UpdateDirectionalShadows(const Vector<DirectionalShadowData>& shadows);
		void UpdatePointShadows(const Vector<PointShadowData>& shadows);
		void UpdateSpotShadows(const Vector<SpotShadowData>& shadows);

	public:
		Vector<DirectionalLightData>& GetDirectionalLights() { return m_dirLights; }
		Vector<PointLightData>& GetPointLights() { return m_pointLights; }
		Vector<SpotLightData>& GetSpotLights() { return m_spotLights; }

		Vector<DirectionalShadowData>& GetDirectionalShadows() { return m_dirShadows; }
		Vector<PointShadowData>& GetPointShadows() { return m_pointShadows; }
		Vector<SpotShadowData>& GetSpotShadows() { return m_spotShadows; }

	public:
		void InitGeometryBuffer(uint32 width, uint32 height);
		GLFramebuffer2D* GetGeometryBuffer() const { return m_geometryBuffer.get(); }
		GLTexture2D* GetGeometryBufferTexture(EGBufferSlot slot) const;

	public:
		void InitPostProcessBuffer(uint32 width, uint32 height);
		ScreenMesh* GetScreenMesh() const { return m_screenMesh.get(); }
		GLFramebuffer2D* GetSceneBuffer() const { return m_sceneBuffer.get(); }
		GLTexture2D* GetSceneColorTexture() const { return m_sceneColorTexture.get(); }

	public:
		void SetCachedTexture(ETextureCache slot, const GLTexture2DPtr& texture);
		GLTexture2D* GetCachedTexture(ETextureCache slot) const;
		void SetDirectionalShadowMap(const GLTexture2DArrayPtr& texture) { m_dirShadowMapArray = texture; }
		GLTexture2DArray* GetDirectionalShadowMap() const { return m_dirShadowMapArray.get(); }
		void SetPointShadowMap(const GLTextureCubeArrayPtr& texture) { m_pointShadowMapArray = texture; }
		GLTextureCubeArray* GetPointShadowMap() const { return m_pointShadowMapArray.get(); }
		void SetSpotShadowMap(const GLTexture2DArrayPtr& texture) { m_spotShadowMapArray = texture; }
		GLTexture2DArray* GetSpotShadowMap() const { return m_spotShadowMapArray.get(); }

	private:
		template <typename T>
		void UpdateSSBO
		(
			GLShaderStorageBufferUPtr& buffer, 
			const Vector<T>& data, 
			uint32 bindingIndex
		);

	private:
		// 전역 설정 대상들
		const Camera* m_currentCamera{ nullptr };
		const SkyLight* m_mainSkyLight{ nullptr };
		GLUniformBufferUPtr m_globalUBO;
		RenderQueue<StaticInstanceData>  m_staticQueue;
		RenderQueue<SkinnedInstanceData> m_skinnedQueue;

		// CPU 측 조명/그림자 데이터 캐시
		Vector<DirectionalLightData> m_dirLights;
		Vector<PointLightData> m_pointLights;
		Vector<SpotLightData> m_spotLights;
		Vector<DirectionalShadowData> m_dirShadows;
		Vector<PointShadowData> m_pointShadows;
		Vector<SpotShadowData> m_spotShadows;

		// Screen 메쉬
		ScreenMeshPtr m_screenMesh;
		GLFramebuffer2DPtr m_sceneBuffer;
		GLTexture2DPtr      m_sceneColorTexture;

		// G-Buffer
		GLFramebuffer2DPtr m_geometryBuffer;
		Vector<GLTexture2DPtr> m_gBufferTextures;
	
		// 조명 전용 SSBO
		GLShaderStorageBufferUPtr m_dirLightSSBO;
		GLShaderStorageBufferUPtr m_pointLightSSBO;
		GLShaderStorageBufferUPtr m_spotLightSSBO;

		// 그림자 전용 SSBO
		GLShaderStorageBufferUPtr m_dirShadowSSBO;
		GLShaderStorageBufferUPtr m_pointShadowSSBO;
		GLShaderStorageBufferUPtr m_spotShadowSSBO;

		// 기타 텍스쳐 캐시 (SSAO, 그림자 등)
		Vector<GLTexture2DPtr> m_cachedTextures;
		GLTexture2DArrayPtr m_dirShadowMapArray;
		GLTextureCubeArrayPtr m_pointShadowMapArray;
		GLTexture2DArrayPtr m_spotShadowMapArray;
	};
}

#include "Rendering/RenderContext.inl"