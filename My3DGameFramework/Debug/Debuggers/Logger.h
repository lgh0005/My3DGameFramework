#pragma once

namespace MGF3D
{
	class Logger
	{
		DECLARE_UTILITIES(Logger)
		using SPDLoggerPtr = std::shared_ptr<spdlog::logger>;

	public:
		static void Init();
		static void Clear();
		static void Flush();

	/*=============================//
	//   default logging methods   //
	//=============================*/
	public:
		template<typename... Args> 
		static void LogTrace(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void LogInfo(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void LogWarn(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void LogError(fmt::format_string<Args...> fmt, Args&&... args);

		template<typename... Args>
		static void LogFatal(fmt::format_string<Args...> fmt, Args&&... args);

	private:
		static SPDLoggerPtr m_logger;
	};
}

#include "Debuggers/Logger.inl"

/*===================//
//   Logger macros   //
//===================*/
#ifdef _DEBUG
#define MGF_LOG_TRACE(...)    MGF3D::Logger::LogTrace(__VA_ARGS__)
#define MGF_LOG_INFO(...)     MGF3D::Logger::LogInfo(__VA_ARGS__)
#define MGF_LOG_WARN(...)     MGF3D::Logger::LogWarn(__VA_ARGS__)
#define MGF_LOG_ERROR(...)    MGF3D::Logger::LogError(__VA_ARGS__)
#define MGF_LOG_FATAL(...)    MGF3D::Logger::LogFatal(__VA_ARGS__)
#define MGF_FLUSH			  MGF3D::Logger::Flush()
#else
#define MGF_LOG_TRACE(...)    ((void)0)
#define MGF_LOG_INFO(...)     ((void)0)
#define MGF_LOG_WARN(...)     ((void)0)
#define MGF_LOG_ERROR(...)    ((void)0)
#define MGF_LOG_FATAL(...)    ((void)0)
#define MGF_FLUSH(...)		  ((void)0)
#endif