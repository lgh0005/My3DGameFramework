#pragma once
#include <spdlog/spdlog.h>

namespace MGF3D
{
	class Logger
	{
		DECLARE_SINGLE(Logger)
		DECLARE_NONINSTANTIABLE(Logger)
		using SPDLoggerPtr = std::shared_ptr<spdlog::logger>;

	public:
		void Init();
		void Clear();
		SPDLoggerPtr GetLogger() { return m_logger; }

	private:
		SPDLoggerPtr m_logger;
	};
}

/*===================//
//   Logger macros   //
//===================*/
#define MGF_LOG_TRACE(...)    ::MGF3D::Logger::Instance().GetLogger()->trace(__VA_ARGS__)
#define MGF_LOG_INFO(...)     ::MGF3D::Logger::Instance().GetLogger()->info(__VA_ARGS__)
#define MGF_LOG_WARN(...)     ::MGF3D::Logger::Instance().GetLogger()->warn(__VA_ARGS__)
#define MGF_LOG_ERROR(...)    ::MGF3D::Logger::Instance().GetLogger()->error(__VA_ARGS__)
#define MGF_LOG_FATAL(...)    ::MGF3D::Logger::Instance().GetLogger()->critical(__VA_ARGS__)