#pragma once
#include "Components/Lights/Light.h"

namespace MGF3D
{
	MGF_CLASS_PTR(SpotLight)
	class SpotLight : public Light
	{
		using Super = Light;

	public:
		virtual ~SpotLight() override;
		SpotLight(ObjectIDHash id, ObjectIDHash ownerID);

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		void SetRange(float range) { m_range = range; }
		float GetRange() const { return m_range; }

		void SetSpotAngles(float innerDegree, float outerDegree);
		float GetInnerCutoff() const { return m_innerCutoff; }
		float GetOuterCutoff() const { return m_outerCutoff; }

	private:
		float m_range{ 10.0f };
		float m_innerCutoff{ 0.0f };
		float m_outerCutoff{ 0.0f };
	};
}
