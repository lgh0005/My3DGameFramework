#pragma once
#include "Entities/Mechanic.h"

namespace MGF3D
{
	MGF_CLASS_PTR(EnvironmentMap)

	MGF_CLASS_PTR(SkyCube)
	class SkyCube : public Mechanic
	{
		MGF_DISABLE_COPY(SkyCube)
		using Super = Mechanic;

	public:
		SkyCube(Component* owner);
		virtual ~SkyCube() override;
		SkyCube(SkyCube&& other) noexcept;
		SkyCube& operator=(SkyCube&& other) noexcept;

	/*================================//
	//   MGF3D Mechanic Custom Type   //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		void SetEnvironmentMap(const EnvironmentMapPtr& envMap) { m_environmentMap = envMap; }
		EnvironmentMapPtr GetEnvironmentMap() const { return m_environmentMap; }
		void BindIBLTextures(uint32 irradSlot, uint32 prefilterSlot, uint32 brdfSlot) const;

	private:
		EnvironmentMapPtr m_environmentMap;
	};
}