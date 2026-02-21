#include "CorePch.h"
#include "MGFName.h"
#include "Managers/NameManager.h"
#include "Utils/StringUtils.h"

namespace MGF3D
{
	MGFName::MGFName() : m_string(nullptr), m_hash(0u) { }
	MGFName::MGFName(cstr s)
	{
		if (StringUtils::IsNullOrEmpty(s))
		{
			m_string = nullptr;
			m_hash = 0u;
			return;
		}

		m_string = NameManager::Instance().Intern(s);
		m_hash = StringHash(m_string);
	}
	MGFName::MGFName(strview sv)
	{
		if (StringUtils::IsNullOrEmpty(sv))
		{
			m_string = nullptr;
			m_hash = 0u;
			return;
		}

		m_string = NameManager::Instance().Intern(sv);
		m_hash = StringHash(sv);
	}

	bool MGFName::IsEmpty() const
	{
		return StringUtils::IsNullOrEmpty(m_string);
	}

	cstr MGFName::CStr() const
	{
		if (m_string == nullptr) return StringUtils::Empty().CStr();
		return m_string;
	}

	uint32 MGFName::GetHashValue() const
	{
		return m_hash;
	}
}