#pragma once
#include "States/MGFStateMachine.h"

namespace MGF3D
{
	enum class EObjectLifecycle : uint8
	{
		None,
		PendingAdd,
		Active,
		PendingKill,
		Dead
	};

	struct MGFObjectInfo
	{
		ObjectIDHash id;
		MGFStateMachine<EObjectLifecycle> lifecycle;
	};
}