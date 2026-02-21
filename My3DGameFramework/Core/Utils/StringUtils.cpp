#include "StringUtils.h"
#include "StringUtils.h"
#include "StringUtils.h"
#include "CorePch.h"
#include "StringUtils.h"

namespace MGF3D
{
	const LString& StringUtils::Empty()
	{
		static const LString emptyStringInstance = LString("");
		return emptyStringInstance;
	}

	bool MGF3D::StringUtils::IsNullOrEmpty(cstr s)
	{
		return (s == nullptr || s[0] == '\0');
	}

	bool MGF3D::StringUtils::IsNullOrEmpty(strview sv)
	{
		return sv.empty();
	}

	bool MGF3D::StringUtils::IsNullOrEmpty(const str& s)
	{
		return s.empty();
	}
}