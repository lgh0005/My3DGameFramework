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
	MGF_CLASS_PTR(GLUniformBuffer)
	MGF_CLASS_PTR(GLShaderStorageBuffer)
	MGF_CLASS_PTR(GLTexture2D)
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
		
		void UpdateDirectionalLights(const Vector<DirectionalLightData>& lights);
		void UpdatePointLights(const Vector<PointLightData>& lights);
		void UpdateSpotLights(const Vector<SpotLightData>& lights);

		void UpdateDirectionalShadows(const Vector<DirectionalShadowData>& shadows);
		void UpdatePointShadows(const Vector<PointShadowData>& shadows);
		void UpdateSpotShadows(const Vector<SpotShadowData>& shadows);

	public:
		ScreenMesh* GetScreenMesh() const { return m_screenMesh.get(); }
		void InitGeometryBuffer(uint32 width, uint32 height);
		GLFramebuffer2D* GetGeometryBuffer() const { return m_geometryBuffer.get(); }
		GLTexture2D* GetGeometryBufferTexture(EGBufferSlot slot) const;

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
		GLUniformBufferUPtr m_globalUBO;
		RenderQueue<StaticInstanceData>  m_staticQueue;
		RenderQueue<SkinnedInstanceData> m_skinnedQueue;

		// Screen 메쉬
		ScreenMeshPtr m_screenMesh;

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
	};
}

#include "Rendering/RenderContext.inl"