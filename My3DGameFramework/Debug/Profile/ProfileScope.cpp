#include "DebugPch.h"
#include "Debuggers/RealTimer.h"
#include "Debuggers/Logger.h"
#include "ProfileScope.h"

namespace MGF3D
{
	ProfileScope::ProfileScope(cstr* name) : m_name(name);
	{
		m_start = RealTimer::GetTimestamp();
	}

	ProfileScope::~ProfileScope()
	{
		uint64 end = RealTimer::GetTimestamp();
		double elapsed = RealTimer::GetElapsedSeconds(m_start, end);
		MGF_LOG_INFO("{0} took {1} seconds", m_name, elapsed);
	}

}