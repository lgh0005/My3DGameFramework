#pragma once
#include "Sources/Resource.h"

namespace MGF3D
{
	class NamedResource : public Resource
	{
		MGF_DISABLE_COPY(NamedResource)

	public:
		NamedResource(StringView name);
		virtual ~NamedResource() override;

	/*==============================//
	//       NamedResource Type     //
	//==============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		const String& GetResourceName() const;
		StringHash GetResourceNameHash() const;

	protected:
		String m_name;
	};
}