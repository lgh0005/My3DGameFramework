#pragma once
#include "Instancing/Lights/DirectionalLightData.h"
#include "Instancing/Lights/PointLightData.h"
#include "Instancing/Lights/SpotLightData.h"
#include "Instancing/Shadows/DirectionalShadowData.h"
#include "Instancing/Shadows/PointShadowData.h"
#include "Instancing/Shadows/SpotShadowData.h"

namespace MGF3D
{
	MGF_CLASS_PTR(RenderContext)
	MGF_CLASS_PTR(Camera)

	MGF_CLASS_PTR(RenderCollector)
	class RenderCollector
	{
	public:
		RenderCollector();
		~RenderCollector();

	public:
		void CollectGlobals(RenderContext* context, const Camera* camera);
		void CollectMeshData(RenderContext* context, const Camera* camera);
	
		void CollectDirectionalLights(RenderContext* context);
		void CollectPointLights(RenderContext* context);
		void CollectSpotLights(RenderContext* context);

		void Clear();

	private:
		Vector<DirectionalLightData> m_dirLights;
		Vector<PointLightData>       m_pointLights;
		Vector<SpotLightData>        m_spotLights;

		Vector<DirectionalShadowData> m_dirShadows;
		Vector<PointShadowData>       m_pointShadows;
		Vector<SpotShadowData>        m_spotShadows;
	};
}