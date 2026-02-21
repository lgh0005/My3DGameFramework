#include "CorePch.h"
#include "MGFName.h"
#include "Utils/StringUtils.h"
#include "Managers/NameManager.h"

namespace MGF3D
{
	MGFName::MGFName() : m_string(nullptr), m_hash(0u) { }
	MGFName::MGFName(cstr s) : m_string(nullptr), m_hash(0u)
	{
		if (!StringUtils::IsNullOrEmpty(s))
		{
			auto name = NameManager::Instance().AddName(s);
			m_string = name.CStr();
			m_hash = name.GetStringHash();
		}
	}
	MGFName::MGFName(strview sv) : m_string(nullptr), m_hash(0u)
	{
		if (!StringUtils::IsNullOrEmpty(sv))
		{
			auto name = NameManager::Instance().AddName(sv);
			m_string = name.CStr();
			m_hash = name.GetStringHash();
		}
	}

	bool MGFName::IsEmpty() const
	{
		return StringUtils::IsNullOrEmpty(m_string);
	}

	cstr MGFName::CStr() const
	{
		if (StringUtils::IsNullOrEmpty(m_string)) 
			return StringUtils::Empty().CStr();
		return m_string;
	}

	StringHash MGFName::GetStringHash() const
	{
		return m_hash;
	}
}