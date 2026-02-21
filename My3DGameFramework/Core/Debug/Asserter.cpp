#include "CorePch.h"
#include "Asserter.h"
#include "Debug/Logger.h"

namespace MGF3D 
{
	void Asserter::_Internal_ReportFailure
	(
		cstr condition,
		cstr message,
		cstr file, int32 line)
	{
		MGF_LOG_FATAL("Assertion Failed!");
		MGF_LOG_FATAL("[Condition] : {0}", condition);
		MGF_LOG_FATAL("[Message]   : {0}", message);
		MGF_LOG_FATAL("[Location]  : {0}({1})", file, line);
		MGF_LOG_FLUSH();
	}

}
