#pragma once
#include "Entities/Component.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Light)
	class Light : public Component
	{
		MGF_DISABLE_COPY(Light)
		using Super = Component;

	public:
		virtual ~Light() override;
		Light(Light&& other) noexcept;
		Light& operator=(Light&& other) noexcept;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		void SetColor(const vec3& color) { m_color = color; }
		const vec3& GetColor() const { return m_color; }

		void SetIntensity(float intensity) { m_intensity = intensity; }
		float GetIntensity() const { return m_intensity; }

		void SetCastShadow(bool cast) { m_castShadow = cast; }
		bool IsCastShadow() const { return m_castShadow; }

	protected:
		Light(ObjectIDHash id, ObjectIDHash ownerID);

		vec3  m_color{ 1.0f, 1.0f, 1.0f };
		float m_intensity{ 1.0f };
		bool  m_castShadow{ false };
	};
}