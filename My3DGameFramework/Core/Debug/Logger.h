#pragma once
#include <spdlog/spdlog.h>

namespace MGF3D
{
	class Logger
	{
		MGF_DECLARE_UTILITIES(Logger)
		using SPDLoggerPtr = SharedPtr<spdlog::logger>;

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

	private:
		static SPDLoggerPtr m_logger;
	};
}

#include "Debug/Logger.inl"