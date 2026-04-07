#include "CorePch.h"
#include "CommonUtils.h"

namespace MGF3D
{
	void CommonUtils::Select(bool condition, const Action<>& trueAction, const Action<>& falseAction)
	{
		if (condition) { if (trueAction) trueAction(); }
		else { if (falseAction) falseAction(); }
	}
}