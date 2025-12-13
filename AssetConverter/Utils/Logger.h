#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Logger
{
	DECLARE_SINGLE(Logger)

public:
	void Init();
	std::shared_ptr<spdlog::logger>& GetLogger() { return m_logger; }

private:
	std::shared_ptr<spdlog::logger> m_logger;
};

/*=================//
//  Logger macros  //
//=================*/
#pragma region LOGGER_MACROS
#define LOGGER Logger::Instance()
#define LOG_TRACE(...)    LOGGER.GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     LOGGER.GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     LOGGER.GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    LOGGER.GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) LOGGER.GetLogger()->critical(__VA_ARGS__)
#pragma endregion