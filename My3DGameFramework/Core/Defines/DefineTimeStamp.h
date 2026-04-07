#pragma once

/*===============================//
//    default timestamp macros   //
//===============================*/
#if defined(_DEBUG)
#define MGF_STOPWATCH_INIT()          MGF3D::Stopwatch::_Internal_Init()
#define MGF_STOPWATCH_GET_TS()        MGF3D::Stopwatch::_Internal_GetTimestamp()
#define MGF_STOPWATCH_ELAPSED(s, e)   MGF3D::Stopwatch::_Internal_GetElapsedSeconds(s, e)
#define MGF_STOPWATCH_TOTAL()         MGF3D::Stopwatch::_Internal_GetTotalTime()
#else
#define MGF_STOPWATCH_INIT()          ((void)0)
#define MGF_STOPWATCH_GET_TS()        (0ULL)
#define MGF_STOPWATCH_ELAPSED(s, e)   (0.0)
#define MGF_STOPWATCH_TOTAL()         (0.0)
#endif