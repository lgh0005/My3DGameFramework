#pragma once
#include "Components/Lights/Light.h"

namespace MGF3D
{
	MGF_CLASS_PTR(PointLight)
	class PointLight : public Light
	{
		using Super = Light;

	public:
		PointLight(ObjectIDHash id, ObjectIDHash ownerID);
		virtual ~PointLight() override;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		void SetRange(float range) { m_range = range; }
		float GetRange() const { return m_range; }

	private:
		float m_range{ 10.0f };
	};
}
