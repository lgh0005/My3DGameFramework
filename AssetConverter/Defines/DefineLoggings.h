#pragma once

/*=============================//
//      Logger Macros          //
//=============================*/
#define MGF_LOG_INIT()          MGF3D::Logger::_Internal_Init()
#define MGF_LOG_CLEAR()         MGF3D::Logger::_Internal_Clear()
#define MGF_LOG_FLUSH()         MGF3D::Logger::_Internal_Flush()
#define MGF_LOG_TRACE(...)      MGF3D::Logger::_Internal_LogTrace(__VA_ARGS__)
#define MGF_LOG_INFO(...)       MGF3D::Logger::_Internal_LogInfo(__VA_ARGS__)
#define MGF_LOG_WARN(...)       MGF3D::Logger::_Internal_LogWarn(__VA_ARGS__)
#define MGF_LOG_ERROR(...)      MGF3D::Logger::_Internal_LogError(__VA_ARGS__)
#define MGF_LOG_FATAL(...)      MGF3D::Logger::_Internal_LogFatal(__VA_ARGS__)