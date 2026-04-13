#pragma once
#include "Entities/Object.h"

namespace MGF3D
{
	class Component : public Object
	{
		MGF_DISABLE_COPY(Component)
		using Super = Object;

	public:
		Component(ObjectIDHash id, ObjectIDHash ownerID);
		virtual ~Component();
		Component(Component&& other) noexcept;
		Component& operator=(Component&& other) noexcept;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		ObjectIDHash GetOwnerID() const;
		void SetEnable(bool enable);
		bool IsEnable() const;

	protected:
		ObjectIDHash  m_ownerID;
	};
}