#include "DebugPch.h"
#include "Debuggers/RealTimer.h"
#include "Debuggers/Logger.h"
#include "ProfileScope.h"

namespace MGF3D
{
	ProfileScope::ProfileScope(cstr name) : m_name(name)
	{
		m_start = RealTimer::_Internal_GetTimestamp();
	}

	ProfileScope::~ProfileScope()
	{
		uint64 end = RealTimer::_Internal_GetTimestamp();
		double elapsed = RealTimer::_Internal_GetElapsedSeconds(m_start, end);
		MGF_LOG_INFO("{0} took {1} seconds", m_name, elapsed);
	}

}