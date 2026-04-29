#pragma once
#include "States/MGFStateMachine.h"

namespace MGF3D
{
	enum class EObjectLifecycle : uint8
	{
		None,
		PendingAdd,  // 생성 대기 (루프에 아직 안 들어감)
		PendingStart,// (Script) Awake는 끝났고, 첫 Update를 돌기 직전 상태
		Active,      // 켜짐 (Update 실행됨)
		Inactive,    // 꺼짐 (Update 건너뜀 - Deactive)
		PendingKill, // 파괴 대기 (이번 프레임 끝에 메모리에서 날아감)
		Dead         // 파괴됨
	};

	struct MGFObjectInfo
	{
		ObjectIDHash id;
		MGFStateMachine<EObjectLifecycle> lifecycle;
	};
}