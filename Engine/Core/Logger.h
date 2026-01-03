#pragma once
#include <spdlog/spdlog.h>

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

/*===================//
//   Logger macros   //
//===================*/
#define LOG_TRACE(...)    ::Logger::Instance().GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Logger::Instance().GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Logger::Instance().GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Logger::Instance().GetLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...)    ::Logger::Instance().GetLogger()->critical(__VA_ARGS__)