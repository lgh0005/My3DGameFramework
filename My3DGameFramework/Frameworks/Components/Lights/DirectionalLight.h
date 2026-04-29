#pragma once
#include "Components/Lights/Light.h"

namespace MGF3D
{
	MGF_CLASS_PTR(DirectionalLight)
	class DirectionalLight : public Light
	{
		using Super = Light;

	public:
		virtual ~DirectionalLight() override;
		DirectionalLight(ObjectIDHash id, ObjectIDHash ownerID);

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		void SetLightSpaceMatrix(const mat4& matrix) { m_lightSpaceMatrix = matrix; }
		const mat4& GetLightSpaceMatrix() const { return m_lightSpaceMatrix; }

	private:
		mat4 m_lightSpaceMatrix{ 1.0f };
	};
}