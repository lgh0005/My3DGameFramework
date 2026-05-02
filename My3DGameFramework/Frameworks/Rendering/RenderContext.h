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

	private:
		template <typename T>
		void UpdateSSBO
		(
			GLShaderStorageBufferUPtr& buffer, 
			const Vector<T>& data, 
			uint32 bindingIndex
		);

	private:
		GLUniformBufferUPtr m_globalUBO;
		RenderQueue<StaticInstanceData>  m_staticQueue;
		RenderQueue<SkinnedInstanceData> m_skinnedQueue;
	
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