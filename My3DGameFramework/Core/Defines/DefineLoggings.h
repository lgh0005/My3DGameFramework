#pragma once

/*=============================//
//      Logger Macros          //
//=============================*/
#ifdef _DEBUG
#define MGF_LOG_INIT()          MGF3D::Logger::_Internal_Init()
#define MGF_LOG_CLEAR()         MGF3D::Logger::_Internal_Clear()
#define MGF_LOG_FLUSH()         MGF3D::Logger::_Internal_Flush()
#define MGF_LOG_TRACE(...)      MGF3D::Logger::_Internal_LogTrace(__VA_ARGS__)
#define MGF_LOG_INFO(...)       MGF3D::Logger::_Internal_LogInfo(__VA_ARGS__)
#define MGF_LOG_WARN(...)       MGF3D::Logger::_Internal_LogWarn(__VA_ARGS__)
#define MGF_LOG_ERROR(...)      MGF3D::Logger::_Internal_LogError(__VA_ARGS__)
#define MGF_LOG_FATAL(...)      MGF3D::Logger::_Internal_LogFatal(__VA_ARGS__)
#else
#define MGF_LOG_INIT()          ((void)0)
#define MGF_LOG_CLEAR()         ((void)0)
#define MGF_LOG_FLUSH()         ((void)0)

#define MGF_LOG_TRACE(...)      ((void)0)
#define MGF_LOG_INFO(...)       ((void)0)
#define MGF_LOG_WARN(...)       ((void)0)
#define MGF_LOG_ERROR(...)      ((void)0)
#define MGF_LOG_FATAL(...)      ((void)0)
#endif