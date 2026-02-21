#pragma once
#include "Common/Clock.h"

namespace MGF3D
{
	class RealTimer
	{
		DECLARE_UTILITIES(RealTimer)

	public:
		static void   _Internal_Init();
		static uint64 _Internal_GetTimestamp();
		static double _Internal_GetElapsedSeconds(uint64 start, uint64 end);
		static double _Internal_GetTotalTime();

#ifdef _DEBUG
	private:
		static Clock::TimePoint s_startTime;
#endif
	};
}

/*==========================//
//    RealTimer Macros      //
//==========================*/
#ifdef _DEBUG
#define MGF_TIMER_INIT()          MGF3D::RealTimer::_Internal_Init()
#define MGF_TIMER_GET_TS()        MGF3D::RealTimer::_Internal_GetTimestamp()
#define MGF_TIMER_ELAPSED(s, e)   MGF3D::RealTimer::_Internal_GetElapsedSeconds(s, e)
#define MGF_TIMER_TOTAL()         MGF3D::RealTimer::_Internal_GetTotalTime()
#else
#define MGF_TIMER_INIT()          ((void)0)
#define MGF_TIMER_GET_TS()        (0ULL)
#define MGF_TIMER_ELAPSED(s, e)   (0.0)
#define MGF_TIMER_TOTAL()         (0.0)
#endif