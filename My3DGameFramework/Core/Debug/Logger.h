#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace MGF3D
{
	class Logger
	{
		DECLARE_UTILITIES(Logger)
		using SPDLoggerPtr = std::shared_ptr<spdlog::logger>;

	public:
		static void _Internal_Init();
		static void _Internal_Clear();
		static void _Internal_Flush();

	/*=============================//
	//   default logging methods   //
	//=============================*/
	public:
		template<typename... Args> 
		static void _Internal_LogTrace(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void _Internal_LogInfo(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void _Internal_LogWarn(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void _Internal_LogError(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void _Internal_LogFatal(fmt::format_string<Args...> fmt, Args&&... args);

#ifdef _DEBUG
	private:
		static SPDLoggerPtr m_logger;
#endif
	};
}

#include "Debug/Logger.inl"

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